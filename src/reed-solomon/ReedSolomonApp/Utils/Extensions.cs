using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;

namespace ReedSolomonApp.Utils
{
    public static class RSExtensions
    {
        public static string GetStringAttributeValue(this Enum value)
        {
            return value.GetType()
                .GetField(value.ToString())
                .GetCustomAttributes(typeof(StringAttribute), false)
                    is IEnumerable<StringAttribute> attrs
                        ? attrs.FirstOrDefault()?.Value
                        : null;
        }

        public static void RunOnUIThread(this Window window, Action action)
        {
            if (window.Dispatcher.CheckAccess())
            {
                action();
            }
            else
                window.Dispatcher.Invoke(action);
        }
    }
}
