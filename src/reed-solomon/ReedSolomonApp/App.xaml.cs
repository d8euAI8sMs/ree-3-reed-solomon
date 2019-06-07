using ReedSolomonApp.Utils;
using System;
using System.Globalization;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Threading;
using System.Windows;

namespace ReedSolomonApp
{
    public partial class App : Application
    {
        private const string ResourceString = "Resources/Strings.";

        public static readonly CultureInfo Rus = new CultureInfo(Langs.Ru.GetStringAttributeValue());
        public static readonly CultureInfo Eng = new CultureInfo(Langs.En.GetStringAttributeValue());
        private static Langs currentLang = Langs.En;

        public static CultureInfo Language
        {
            get
            {
                CheckUIThread();
                return Thread.CurrentThread.CurrentUICulture;
            }
            set
            {
                CheckUIThread();

                if (value == null)
                    throw new ArgumentNullException();

                if (value != Thread.CurrentThread.CurrentUICulture)
                {
                    Thread.CurrentThread.CurrentUICulture = value;

                    var dict = new ResourceDictionary();
                    if (value.Equals(Rus))
                    {
                        dict.Source = new Uri(string.Format("{0}{1}.xaml", ResourceString, value.Name), UriKind.Relative);
                    }
                    else if (value.Equals(Eng))
                    {
                        dict.Source = new Uri(string.Format("{0}xaml", ResourceString), UriKind.Relative);
                    }
                    else
                        throw new NotSupportedException();

                    var oldDict = Current.Resources.MergedDictionaries
                        .FirstOrDefault(d => string.Equals(d.Source.OriginalString,
                            string.Format("{0}{1}.xaml", ResourceString, currentLang.GetStringAttributeValue())))
                        ?? Current.Resources.MergedDictionaries
                            .FirstOrDefault(d => string.Equals(d.Source.OriginalString,
                                string.Format("{0}xaml", ResourceString)));

                    if (oldDict != null)
                    {
                        int ind = Current.Resources.MergedDictionaries.IndexOf(oldDict);
                        Current.Resources.MergedDictionaries.Remove(oldDict);
                        Current.Resources.MergedDictionaries.Insert(ind, dict);
                    }
                    else
                        Current.Resources.MergedDictionaries.Add(dict);
                }
            }
        }

        public static T FindResource<T>(string key) where T : class => Current.FindResource(key) as T;

        public static string GetStringResource(string key) => FindResource<string>(key) ?? string.Empty;

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void CheckUIThread()
        {
#if DEBUG
            if (Thread.CurrentThread != Current.Dispatcher.Thread) 
            {
                throw new ThreadStateException("Assertion failed: not on the UI thread!");
            }
#endif
        }

        public enum Langs
        {
            [String("en-US")]
            En,
            [String("ru-RU")]
            Ru
        }
    }
}
