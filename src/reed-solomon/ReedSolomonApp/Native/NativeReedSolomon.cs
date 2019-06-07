using ReedSolomonApp.Utils;
using System.Collections.Generic;

namespace ReedSolomonApp
{
    public static class NativeReedSolomon
    {
        public const ushort DefaultPolinomialGenerator = 11;
#if NOISE_DECODE_ENABLE
        public const ulong DefaultErrorCount = 3UL;
        public const float DefaultErrorFrequence = 0.5f;
#endif

        public enum RSCode
        {
            [String("Success")]
            Success = 0,
            [String("Out of memory")]
            OutOfMemory,
            [String("Insufficient buffer space")]
            InsufficientBufferSpace,
            [String("Invalid Argument")]
            InvalidArgument,
            [String("Unhandled")]
            Unhandled
        }

        public class RSEventArgs
        {
            public RSCode Code { get; }
            public IEnumerable<byte> Result { get; }

            public RSEventArgs(RSCode code, IEnumerable<byte> result)
            {
                Code = code;
                Result = result;
            }
        }

#if NOISE_DECODE_ENABLE
        public sealed class RSExtEventArgs : RSEventArgs
        {
            public bool IsNoised { get; }

            public RSExtEventArgs(RSCode code, IEnumerable<byte> result, bool isNoised)
                : base(code, result) => IsNoised = isNoised;
        }
#endif
    }
}