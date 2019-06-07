using System.Text;
using System.Windows;
using System.Windows.Controls;
using ReedSolomonApp.Utils;
using System.IO;
using System;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using static ReedSolomonApp.NativeReedSolomon;
using static ReedSolomonApp.App;

namespace ReedSolomonApp
{
    public partial class MainWindow : Window
    {
        private const string DefaultFilePath = "encoded_bytes.dat";

        private NativeReedSolomonImpl reedSolomonImpl;
        private string inputText;
        private bool isBytesView;
        private byte[] encodedBytes;
#if NOISE_DECODE_ENABLE
        private byte[] noisedBytes;
        private byte[] decodedBytes;
#endif

        private NativeReedSolomonImpl ReedSolomonImpl
        {
            get
            {
                CheckUIThread();
                return reedSolomonImpl;
            }
            set
            {
                CheckUIThread();
                if (reedSolomonImpl != value)
                {
                    if (reedSolomonImpl != null)
                    {
                        reedSolomonImpl.Encoded -= OnReedSolomonEncoded;
#if NOISE_DECODE_ENABLE
                        reedSolomonImpl.Noised -= OnReedSolomonNoised;
                        reedSolomonImpl.Decoded -= OnReedSolomonDecoded;
#endif
                        reedSolomonImpl.Dispose();
                    }

                    reedSolomonImpl = value;
                    if (reedSolomonImpl != null)
                    {
                        reedSolomonImpl.Encoded += OnReedSolomonEncoded;
#if NOISE_DECODE_ENABLE
                        reedSolomonImpl.Noised += OnReedSolomonNoised;
                        reedSolomonImpl.Decoded += OnReedSolomonDecoded;
#endif
                    }
                }
            }
        }

        public MainWindow()
        {
            InitializeComponent();

#if !NOISE_DECODE_ENABLE
            noiseButton.Visibility = decodeButton.Visibility = errFrequencyTextBox.Visibility
                = errCountTextBox.Visibility = errFrequencyTextBlock.Visibility
                = errCountTextBlock.Visibility = Visibility.Collapsed;
#endif
        }

        private void OnReedSolomonEncoded(object sender, RSEventArgs e) => this.RunOnUIThread(() =>
        {
            if (e.Code == RSCode.Success)
            {
                encodedBytes = e.Result.ToArray();
                resultTextBox.Text = GetByteStr(encodedBytes);
                serialiseButton.IsEnabled = resetButton.IsEnabled
                    = inputTextBox.IsReadOnly = true;
#if NOISE_DECODE_ENABLE
                noiseButton.IsEnabled = decodeButton.IsEnabled = true;
#endif
            }
            else
                OnReedSolomonError(e.Code);
        });

#if NOISE_DECODE_ENABLE
        private void OnReedSolomonNoised(object sender, RSEventArgs e) => this.RunOnUIThread(() =>
        {
            noisedBytes = e.Result.ToArray();
            if (e.Code == RSCode.Success)
            {
                resultTextBox.Text = GetByteStr(noisedBytes);
                if (decodedBytes != null)
                    MessageBox.Show(this, GetStringResource("noise_postponed_message"), GetStringResource("attantion"));
            }
            else
                OnReedSolomonError(e.Code);
        });

        private void OnReedSolomonDecoded(object sender, RSExtEventArgs e) => this.RunOnUIThread(() =>
        {
            decodedBytes = e.Result.ToArray();
            if (e.Code == RSCode.Success)
            {
                resultTextBox.Text = GetByteStr(decodedBytes);
            }
            else
                OnReedSolomonError(e.Code);

            MessageBox.Show(this, e.IsNoised
                    ? GetStringResource("noise_decoded_message")
                    : GetStringResource("clear_decoded_message"),
                GetStringResource("success"));
        });
#endif

        private void OnEncodeClicked(object sender, RoutedEventArgs e)
        {
            CheckUIThread();

            if (encodedBytes == null)
            {
                var message = Encoding.ASCII.GetBytes(string.IsNullOrEmpty(inputText) ? inputTextBox.Text : inputText);
                ReedSolomonImpl = new NativeReedSolomonImpl(message);

                if (string.IsNullOrEmpty(polGenTextBox.Text))
                {
                    ReedSolomonImpl.PolynomialGenerator = DefaultPolinomialGenerator;
                }
                else if (ushort.TryParse(polGenTextBox.Text, out var pg))
                {
                    ReedSolomonImpl.PolynomialGenerator = pg;
                }
                else
                {
                    OnIncorrectData(nameof(ReedSolomonImpl.PolynomialGenerator));
                    ReedSolomonImpl = null;
                    return;
                }
                ReedSolomonImpl.EncodeAsync();
            }
            else
                resultTextBox.Text = GetByteStr(encodedBytes);
        }

        private void OnNoiseClicked(object sender, RoutedEventArgs e)
        {
            CheckUIThread();

#if NOISE_DECODE_ENABLE
            if (noisedBytes == null)
            {
                if (string.IsNullOrEmpty(errCountTextBox.Text))
                {
                    ReedSolomonImpl.ErrorCount = DefaultErrorCount;
                }
                else if (ushort.TryParse(errCountTextBox.Text, out var ec))
                {
                    ReedSolomonImpl.ErrorCount = ec;
                }
                else
                {
                    OnIncorrectData(nameof(ReedSolomonImpl.ErrorCount));
                    return;
                }

                if (string.IsNullOrEmpty(errFrequencyTextBox.Text))
                {
                    ReedSolomonImpl.ErrorFrequency = DefaultErrorFrequence;
                }
                else if (ushort.TryParse(errFrequencyTextBox.Text, out var ef))
                {
                    ReedSolomonImpl.ErrorFrequency = ef;
                }
                else
                {
                    OnIncorrectData(nameof(ReedSolomonImpl.ErrorFrequency));
                    return;
                }
                ReedSolomonImpl.NoiseAsync();
            }
            else
                resultTextBox.Text = GetByteStr(noisedBytes);
#endif
        }

        private void OnDecodeClicked(object sender, RoutedEventArgs e)
        {
            CheckUIThread();

#if NOISE_DECODE_ENABLE
            if (decodedBytes == null)
            {
                if (string.IsNullOrEmpty(polGenTextBox.Text))
                {
                    ReedSolomonImpl.PolynomialGenerator = DefaultPolinomialGenerator;
                }
                else if (ushort.TryParse(polGenTextBox.Text, out var pg))
                {
                    ReedSolomonImpl.ErrorCount = pg;
                }
                else
                {
                    OnIncorrectData(nameof(ReedSolomonImpl.PolynomialGenerator));
                    return;
                }
                ReedSolomonImpl.DecodeAsync();
            }
            else
                resultTextBox.Text = GetByteStr(decodedBytes);
#endif
        }

        private void OnResetClicked(object sender, RoutedEventArgs e)
        {
            CheckUIThread();
            if (MessageBoxResult.Yes == MessageBox.Show(this, GetStringResource("reset_message"),
                GetStringResource("reset"), MessageBoxButton.YesNo))
            {
                ReedSolomonImpl = null;
                encodedBytes = null;
#if NOISE_DECODE_ENABLE
                noisedBytes = decodedBytes = null;
#endif
                inputTextBox.IsReadOnly = encodeButton.IsEnabled = decodeButton.IsEnabled
                    = noiseButton.IsEnabled = changeTextBoxViewButton.IsEnabled
                    = resetButton.IsEnabled = isBytesView = false;
                inputTextBox.Text = resultTextBox.Text = inputText = string.Empty;
                changeTextBoxViewButton.Content = GetStringResource("bytes");
            }
        }

        private void OnSaveClicked(object sender, RoutedEventArgs e)
        {
            CheckUIThread();
            SerializeBytes(encodedBytes);
            serialiseButton.IsEnabled = false;
        }

        private void OnChangeTextViewClicked(object sender, RoutedEventArgs e)
        {
            CheckUIThread();
            if (isBytesView)
            {
                changeTextBoxViewButton.Content = GetStringResource("bytes");
                inputTextBox.Text = inputText;
                inputTextBox.IsReadOnly = isBytesView = false;
            }
            else
            {
                changeTextBoxViewButton.Content = GetStringResource("text");
                inputText = inputTextBox.Text;
                inputTextBox.Text = BitConverter.ToString(Encoding.ASCII.GetBytes(inputText))
                    .Replace('-', ' ');
                inputTextBox.IsReadOnly = isBytesView = true;
            }
        }

        private void OnWindowClosing(object sender, CancelEventArgs e) => ReedSolomonImpl = null;

        private void OnInputTextChanged(object sender, TextChangedEventArgs e)
        {
            CheckUIThread();
            changeTextBoxViewButton.IsEnabled = encodeButton.IsEnabled = !string.IsNullOrEmpty(inputTextBox.Text);
        }

        private void OnLangChanged(object sender, SelectionChangedEventArgs e)
        {
            if (sender is ComboBox cb)
            {
                if (IsLoaded)
                {
                    switch (cb.SelectedIndex)
                    {
                        case (int)Langs.Ru:
                            App.Language = Rus;
                            break;
                        case (int)Langs.En:
                            App.Language = Eng;
                            break;
                        default:
                            throw new NotSupportedException();
                    }
                    changeTextBoxViewButton.Content = isBytesView
                        ? GetStringResource("text")
                        : GetStringResource("bytes");
                }
            }
            else
                throw new InvalidOperationException();
        }

        private void SerializeBytes(byte[] bytes)
        {
            string path = string.IsNullOrEmpty(filePathTextBox.Text)
                ? DefaultFilePath
                : filePathTextBox.Text.Trim();

            using (var fs = new FileStream(path, FileMode.OpenOrCreate))
            {
                try
                {
                    fs.Write(bytes, 0, bytes.Length);
                    MessageBox.Show(this, string.Format("{0} {1}", GetStringResource("saved_at"), path),
                        GetStringResource("success"));
                }
                catch (Exception ex)
                {
                    MessageBox.Show(this, ex.Message, GetStringResource("error"));
                }
            }
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private MessageBoxResult OnIncorrectData(string arg) => MessageBox.Show(this, arg, "incorrect_data");

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private MessageBoxResult OnReedSolomonError(RSCode code)
            => MessageBox.Show(this, code.GetStringAttributeValue(), GetStringResource("error"));

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private string GetByteStr(byte[] bytes) => BitConverter.ToString(bytes).Replace('-', ' ');
    }
}