#include "stdafx.h"
#include "CppUnitTest.h"

#include <reed-solomon/gf28.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework
{
    template<> inline std::wstring ToString<word_t>(const word_t & t) { RETURN_WIDE_STRING(t); }
} } }

namespace reedsolomontest
{		
	TEST_CLASS(gf28_test)
	{
	public:
		
		TEST_METHOD(gflog2)
		{
            Assert::AreEqual(byte_t(0), gf28::gflog2(0), L"", LINE_INFO());
            Assert::AreEqual(byte_t(0), gf28::gflog2(1), L"", LINE_INFO());
            Assert::AreEqual(byte_t(1), gf28::gflog2(2), L"", LINE_INFO());
            Assert::AreEqual(byte_t(1), gf28::gflog2(3), L"", LINE_INFO());
            Assert::AreEqual(byte_t(2), gf28::gflog2(4), L"", LINE_INFO());
            Assert::AreEqual(byte_t(2), gf28::gflog2(7), L"", LINE_INFO());
            Assert::AreEqual(byte_t(3), gf28::gflog2(8), L"", LINE_INFO());
            Assert::AreEqual(byte_t(3), gf28::gflog2(15), L"", LINE_INFO());
            Assert::AreEqual(byte_t(7), gf28::gflog2(255), L"", LINE_INFO());
            Assert::AreEqual(byte_t(8), gf28::gflog2(256), L"", LINE_INFO());
		}

		TEST_METHOD(gfmul)
		{
            Assert::AreEqual(word_t(0),        gf28::gfmul(0, 0), L"", LINE_INFO());
            Assert::AreEqual(word_t(0),        gf28::gfmul(0, 1), L"", LINE_INFO());
            Assert::AreEqual(word_t(0),        gf28::gfmul(1, 0), L"", LINE_INFO());
            Assert::AreEqual(word_t(1),        gf28::gfmul(1, 1), L"", LINE_INFO());
            Assert::AreEqual(word_t(2),        gf28::gfmul(1, 2), L"", LINE_INFO());
            Assert::AreEqual(word_t(2),        gf28::gfmul(2, 1), L"", LINE_INFO());
            Assert::AreEqual(word_t(123),      gf28::gfmul(1, 123), L"", LINE_INFO());
            Assert::AreEqual(word_t(123),      gf28::gfmul(123, 1), L"", LINE_INFO());
            Assert::AreEqual(word_t(4),        gf28::gfmul(2, 2), L"", LINE_INFO());
            Assert::AreEqual(word_t(123 << 1), gf28::gfmul(2, 123), L"", LINE_INFO());
            Assert::AreEqual(word_t(27),       gf28::gfmul(5, 7), L"", LINE_INFO());
            Assert::AreEqual(word_t(6720),     gf28::gfmul(88, 120), L"", LINE_INFO());
		}

		TEST_METHOD(gfmod)
		{
            Assert::AreEqual(byte_t(0),   (byte_t)gf28::gfmod(0, 1), L"", LINE_INFO());
            Assert::AreEqual(byte_t(0),   (byte_t)gf28::gfmod(1, 1), L"", LINE_INFO());
            Assert::AreEqual(byte_t(1),   (byte_t)gf28::gfmod(1, 2), L"", LINE_INFO());
            Assert::AreEqual(byte_t(1),   (byte_t)gf28::gfmod(1, 3), L"", LINE_INFO());
            Assert::AreEqual(byte_t(1),   (byte_t)gf28::gfmod(2, 3), L"", LINE_INFO());
            Assert::AreEqual(byte_t(1),   (byte_t)gf28::gfmod(3, 2), L"", LINE_INFO());
            Assert::AreEqual(byte_t(6),   (byte_t)gf28::gfmod(27, 11), L"", LINE_INFO());
		}

		TEST_METHOD(gfinit)
		{
            Assert::AreEqual(errc_t(0), (errc_t)gf28::gfinit(11), L"", LINE_INFO());

            Assert::AreEqual(word_t(11), gf28::gfscal_t::gv, L"", LINE_INFO());
            Assert::AreEqual(byte_t(2), gf28::gfscal_t::pe, L"", LINE_INFO());

            Assert::AreEqual(byte_t(0), gf28::gfscal_t::multable[0][0], L"", LINE_INFO());
            Assert::AreEqual(byte_t(0), gf28::gfscal_t::multable[1][0], L"", LINE_INFO());
            Assert::AreEqual(byte_t(0), gf28::gfscal_t::multable[0][1], L"", LINE_INFO());
            Assert::AreEqual(byte_t(1), gf28::gfscal_t::multable[1][1], L"", LINE_INFO());
            Assert::AreEqual(byte_t(4), gf28::gfscal_t::multable[2][2], L"", LINE_INFO());
            Assert::AreEqual(byte_t(6), gf28::gfscal_t::multable[5][7], L"", LINE_INFO());
            Assert::AreEqual(byte_t(6), gf28::gfscal_t::multable[7][5], L"", LINE_INFO());
            
            Assert::AreEqual(byte_t(1), gf28::gfscal_t::divtable[1][1], L"", LINE_INFO());
            Assert::AreEqual(byte_t(2), gf28::gfscal_t::divtable[2][1], L"", LINE_INFO());
            Assert::AreEqual(byte_t(5), gf28::gfscal_t::divtable[6][7], L"", LINE_INFO());
            Assert::AreEqual(byte_t(7), gf28::gfscal_t::divtable[6][5], L"", LINE_INFO());
            
            Assert::AreEqual(byte_t(1), gf28::gfscal_t::powtable[1][1], L"", LINE_INFO());
            Assert::AreEqual(byte_t(1), gf28::gfscal_t::powtable[2][0], L"", LINE_INFO());
            Assert::AreEqual(byte_t(2), gf28::gfscal_t::powtable[2][1], L"", LINE_INFO());
            Assert::AreEqual(byte_t(4), gf28::gfscal_t::powtable[2][2], L"", LINE_INFO());
            Assert::AreEqual(byte_t(3), gf28::gfscal_t::powtable[2][3], L"", LINE_INFO());
            
            Assert::AreEqual(byte_t(1), gf28::gfscal_t::pwitable[1][1], L"", LINE_INFO());
            Assert::AreEqual(byte_t(1), gf28::gfscal_t::pwitable[2][0], L"", LINE_INFO());
            Assert::AreEqual(byte_t(5), gf28::gfscal_t::pwitable[2][1], L"", LINE_INFO());
            Assert::AreEqual(byte_t(7), gf28::gfscal_t::pwitable[2][2], L"", LINE_INFO());
            Assert::AreEqual(byte_t(6), gf28::gfscal_t::pwitable[2][3], L"", LINE_INFO());
		}

		TEST_METHOD(gfscal_t_opmul)
		{
            Assert::AreEqual(errc_t(0), (errc_t)gf28::gfinit(11), L"", LINE_INFO());

            Assert::AreEqual(byte_t(6), (gf28::gfscal_t(5) * gf28::gfscal_t(7)).val, L"", LINE_INFO());
            Assert::AreEqual(byte_t(6), (gf28::gfscal_t(7) * gf28::gfscal_t(5)).val, L"", LINE_INFO());
		}

		TEST_METHOD(gfscal_t_opdiv)
		{
            Assert::AreEqual(errc_t(0), (errc_t)gf28::gfinit(11), L"", LINE_INFO());

            Assert::AreEqual(byte_t(7), (gf28::gfscal_t(6) / gf28::gfscal_t(5)).val, L"", LINE_INFO());
            Assert::AreEqual(byte_t(5), (gf28::gfscal_t(6) / gf28::gfscal_t(7)).val, L"", LINE_INFO());
		}

		TEST_METHOD(gfscal_t_oppow)
		{
            Assert::AreEqual(errc_t(0), (errc_t)gf28::gfinit(11), L"", LINE_INFO());
            
            Assert::AreEqual(byte_t(1), (gf28::gfscal_t(2) ^ 0).val, L"", LINE_INFO());
            Assert::AreEqual(byte_t(3), (gf28::gfscal_t(2) ^ 3).val, L"", LINE_INFO());
            Assert::AreEqual(byte_t(6), (gf28::gfscal_t(2) ^ (-3)).val, L"", LINE_INFO());
		}
	};
}