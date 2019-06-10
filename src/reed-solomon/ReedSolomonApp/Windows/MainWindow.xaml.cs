using System.Text;
using System.Windows;
using System.Windows.Controls;
using ReedSolomonApp.Utils;
using System.IO;
using System;
using System.ComponentModel;
using System.Linq;
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
        private string encodedText;
        private ViewState viewState = ViewState.None;
#if NOISE_DECODE_ENABLE
        private byte[] noisedBytes;
        private string noisedText;
        private byte[] decodedBytes;
        private string decodedText;
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
                viewState = ViewState.Encode;
                encodedBytes = e.Result.ToArray();
                encodedText = GetStringFromBytes(encodedBytes);
                resultTextBox.Text = GetResultTextBoxText(viewState);
                serialiseButton.IsEnabled = resetButton.IsEnabled = true;
                inputTextBox.IsReadOnly = false;
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
                viewState = ViewState.Noise;
                noisedBytes = e.Result.ToArray();
                noisedText = GetStringFromBytes(noisedBytes);
                resultTextBox.Text = GetResultTextBoxText(viewState);
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
                viewState = ViewState.Decode;
                decodedBytes = e.Result.ToArray();
                decodedText = GetStringFromBytes(decodedBytes);
                resultTextBox.Text = GetResultTextBoxText(viewState);
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
            if (encodedBytes == null || MessageBoxResult.Yes == MessageBox.Show(this,
                GetStringResource("reset_message"), GetStringResource("encode"), MessageBoxButton.YesNo))
            {
                encodedBytes = null;
#if NOISE_DECODE_ENABLE
                noisedBytes = decodedBytes = null;
#endif
                inputText = string.Empty;
                var message = Encoding.ASCII.GetBytes(string.IsNullOrEmpty(inputText) ? inputTextBox.Text : inputText);
                ReedSolomonImpl = new NativeReedSolomonImpl(message);

                if (string.IsNullOrEmpty(polGenTextBox.Text))
                {
                    ReedSolomonImpl.GeneratorPolynomial = DefaultGeneratorPolynomial;
                }
                else if (ushort.TryParse(polGenTextBox.Text, out var pg))
                {
                    ReedSolomonImpl.GeneratorPolynomial = pg;
                }
                else
                {
                    OnIncorrectData(nameof(ReedSolomonImpl.GeneratorPolynomial));
                    ReedSolomonImpl = null;
                    return;
                }
                ReedSolomonImpl.EncodeAsync();
            }
            else
            {
                viewState = ViewState.Encode;
                resultTextBox.Text = GetResultTextBoxText(viewState);
            }
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
                    ReedSolomonImpl.ErrorFrequency = DefaultErrorFrequency;
                }
                else if (float.TryParse(errFrequencyTextBox.Text, out var ef))
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
            {
                viewState = ViewState.Noise;
                resultTextBox.Text = GetResultTextBoxText(viewState);
            }
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
                    ReedSolomonImpl.GeneratorPolynomial = DefaultGeneratorPolynomial;
                }
                else if (ushort.TryParse(polGenTextBox.Text, out var pg))
                {
                    ReedSolomonImpl.GeneratorPolynomial = pg;
                }
                else
                {
                    OnIncorrectData(nameof(ReedSolomonImpl.GeneratorPolynomial));
                    return;
                }
                ReedSolomonImpl.DecodeAsync();
            }
            else
            {
                viewState = ViewState.Decode;
                resultTextBox.Text = GetResultTextBoxText(viewState);
            }
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
                inputTextBox.Text = GetHEXString(Encoding.ASCII.GetBytes(inputText));
                inputTextBox.IsReadOnly = isBytesView = true;
            }
            resultTextBox.Text = GetResultTextBoxText(viewState);
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
                    App.Language = (Langs)Enum.ToObject(typeof(Langs), cb.SelectedIndex);
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

        private MessageBoxResult OnIncorrectData(string arg) => MessageBox.Show(this, arg, "incorrect_data");

        private MessageBoxResult OnReedSolomonError(RSCode code)
            => MessageBox.Show(this, code.GetStringAttributeValue(), GetStringResource("error"));

        private string GetHEXString(byte[] bytes) => BitConverter.ToString(bytes).Replace('-', ' ');

        private string GetStringFromBytes(byte[] bytes) => Encoding.ASCII.GetString(bytes);

        private string GetResultTextBoxText(ViewState viewState)
        {
            switch (viewState)
            {
                case ViewState.None:
                    return string.Empty;
                case ViewState.Encode:
                    return isBytesView ? GetHEXString(encodedBytes) : encodedText;
#if NOISE_DECODE_ENABLE
                case ViewState.Noise:
                    return isBytesView ? GetHEXString(noisedBytes) : noisedText;
                case ViewState.Decode:
                    return isBytesView ? GetHEXString(decodedBytes) : decodedText;
#endif
                default:
                    throw new InvalidEnumArgumentException(string.Format("Unhandled argument: {0}", viewState.ToString()));
            }
        }

        private enum ViewState
        {
            None,
            Encode,
#if NOISE_DECODE_ENABLE
            Noise,
            Decode
#endif
        }
    }
}