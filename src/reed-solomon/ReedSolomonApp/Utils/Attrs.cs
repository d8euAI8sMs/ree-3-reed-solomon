using System;

namespace ReedSolomonApp.Utils
{
    public class StringAttribute : Attribute
    {
        public string Value { get; }

        public StringAttribute(string value) => Value = value;
    }
}
