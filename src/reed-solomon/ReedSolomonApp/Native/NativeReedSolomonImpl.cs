using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using static ReedSolomonApp.NativeReedSolomon;

namespace ReedSolomonApp
{
    public sealed class NativeReedSolomonImpl
    {
        private RSBuffer input;
        private RSBuffer outEncoded;
#if NOISE_DECODE_ENABLE
        private RSBuffer outDecoded;
        private bool isNoised;
#endif

        public byte[] Message { get; private set; }
        public ushort PolynomialGenerator { get; set; }
#if NOISE_DECODE_ENABLE
        public ulong ErrorCount { get; set; }
        public float ErrorFrequency { get; set; }
#endif

        public event EventHandler<RSEventArgs> Encoded;
#if NOISE_DECODE_ENABLE
        public event EventHandler<RSEventArgs> Noised;
        public event EventHandler<RSExtEventArgs> Decoded;
#endif

        public NativeReedSolomonImpl(IEnumerable<byte> message)
        {
            try
            {
                Message = message.ToArray();
                input = new RSBuffer
                {
                    Buffer = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(byte)) * Message.Length),
                    Length = Message.Length
                };
                Marshal.Copy(Message, 0, input.Buffer, Message.Length);
            }
            catch
            {
                throw new Exception("Can not create new instance of ReedSolomonApp.NativeReedSolomonImpl");
            }
        }

        private (RSCode, IEnumerable<byte>) Encode()
        {
            if (Message == null)
            {
#if DEBUG
                throw new NullReferenceException("Message equals to null");
#else
                return (RSCode.Unhandled, null);
#endif
            }

            outEncoded = new RSBuffer();
            var code =  ReedSolomonEncode(input, PolynomialGenerator, ref outEncoded);
            var encodedBytes = new byte[outEncoded.Length];
            if (code == RSCode.Success)
            {
                Marshal.Copy(outEncoded.Buffer, encodedBytes, 0, outEncoded.Length);
            }
            return (code, encodedBytes);
        }

        public async void EncodeAsync()
        {
            var (code, bytes) = await Task.Run(() => Encode());
            Encoded?.Invoke(this, new RSEventArgs(code, bytes));
        }

#if NOISE_DECODE_ENABLE
        private (RSCode, IEnumerable<byte>) Noise()
        {
            if (outEncoded.Buffer == IntPtr.Zero)
            {
#if DEBUG
                throw new NullReferenceException("outEncoded equals to ZERO");
#else
                return (RSCode.Unhandled, null);
#endif
            }

            var code = ReedSolomonNoise(outEncoded, PolynomialGenerator, ErrorCount, ErrorFrequency);
            var noisedBytes = new byte[outEncoded.Length];
            if (code == RSCode.Success)
            {
                Marshal.Copy(outEncoded.Buffer, noisedBytes, 0, outEncoded.Length);
                isNoised = true;
            }
            return (code, noisedBytes);
        }

        public async void NoiseAsync()
        {
            var (code, bytes) = await Task.Run(() => Noise());
            Noised?.Invoke(this, new RSEventArgs(code, bytes));
        }

        private (RSCode, IEnumerable<byte>) Decode()
        {
            if (outEncoded.Buffer == IntPtr.Zero)
            {
#if DEBUG
                throw new NullReferenceException("outEncoded equals to ZERO");
#else
                return (RSCode.Unhandled, null);
#endif
            }

            outDecoded = new RSBuffer();
            var code = ReedSolomonDecode(outEncoded, PolynomialGenerator, ref outDecoded);
            var decodedBytes = new byte[outDecoded.Length];
            if (code == RSCode.Success)
            {
                Marshal.Copy(outDecoded.Buffer, decodedBytes, 0, outDecoded.Length);
            }
            return (code, decodedBytes);
        }

        public async void DecodeAsync()
        {
            var (code, bytes) = await Task.Run(() => Decode());
            Decoded?.Invoke(this, new RSExtEventArgs(code, bytes, isNoised));
        }
#endif

        public void Dispose()
        {
            if (outEncoded.Buffer != IntPtr.Zero)
                ReedSolomonFree(outEncoded);
#if NOISE_DECODE_ENABLE
            if (outDecoded.Buffer != IntPtr.Zero)
                ReedSolomonFree(outDecoded);
#endif
            if (input != IntPtr.Zero)
                Marshal.FreeHGlobal(input);
        }

        [DllImport("reed-solomon.dll", EntryPoint = "ReedSolomonFree")]
        private static extern RSCode ReedSolomonFree(IntPtr ptr);

        [DllImport("reed-solomon.dll", EntryPoint = "ReedSolomonEncode")]
        private static extern RSCode ReedSolomonEncode(RSBuffer input, ushort gv, ref RSBuffer output);

#if NOISE_DECODE_ENABLE
        [DllImport("reed-solomon.dll", EntryPoint = "ReedSolomonNoise")]
        private static extern RSCode ReedSolomonNoise(RSBuffer buf, ushort gv, ulong ec, float freq);

        [DllImport("reed-solomon.dll", EntryPoint = "ReedSolomonDecode")]
        private static extern RSCode ReedSolomonDecode(RSBuffer input, ushort gv, ref RSBuffer output);
#endif

        [StructLayout(LayoutKind.Sequential)]
        private struct RSBuffer
        {
            public int Length;
            public IntPtr Buffer;

            public static implicit operator IntPtr(RSBuffer buf) => buf.Buffer;
        }
    }
}