#include "stdafx.h"
#include "CppUnitTest.h"

#include <reed-solomon/api.h>

#include <array>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace reedsolomontest
{		
	TEST_CLASS(rs_test)
	{
	public:
		
		TEST_METHOD(rs_encode_encode)
        {
            std::array<byte_t, 9> msg = {{ 4,6,7,0,0,0,4,6,7 }};
            std::array<byte_t, 21> out = {{ 7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7 }};
            std::array<byte_t, 21> res = {{ 5,2,5,3,3,2,4,0,0,0,0,0,0,0,5,2,5,3,3,2,4 }};

            buf_t inb = { 9, msg.data() }, outb = { 21, out.data() };

            errc_t err = rs_encode(inb, 11, outb);
            
            Assert::AreEqual(errc_t(0), err, L"", LINE_INFO());
            Assert::IsTrue(out == res, L"", LINE_INFO());
        }
		
		TEST_METHOD(rs_encode_alloc)
        {
            std::array<byte_t, 9> msg = {{ 4,6,7,0,0,0,4,6,7 }};
            std::array<byte_t, 21> res = {{ 5,2,5,3,3,2,4,0,0,0,0,0,0,0,5,2,5,3,3,2,4 }};

            buf_t inb = { 9, msg.data() }, outb = { 0, nullptr };

            __try
            {
                errc_t err = rs_encode(inb, 11, outb);
            
                Assert::AreEqual(errc_t(0), err, L"", LINE_INFO());
                Assert::AreEqual(21, outb.len, L"", LINE_INFO());
                Assert::AreNotEqual(0, outb.len, L"", LINE_INFO());

                Assert::IsTrue(std::equal(res.data(), res.data() + 21, outb.buf), L"", LINE_INFO());
            }
            __finally
            {
                rs_free(outb.buf);
            }
        }
		
		TEST_METHOD(rs_encode_insuff)
        {
            std::array<byte_t, 9> msg = {{ 4,6,7,0,0,0,4,6,7 }};

            buf_t inb = { 9, msg.data() }, outb = { 1, msg.data() };

            errc_t err = rs_encode(inb, 11, outb);

            Assert::AreEqual(errc_t(2), err, L"", LINE_INFO());
        }
		
		TEST_METHOD(rs_decode_decode)
        {
            std::array<byte_t, 21> msg = {{ 5,2,5,3,3,2,4,0,0,0,0,0,0,0,5,2,5,3,3,2,4 }};
            std::array<byte_t, 9>  out = {{ 7,7,7,7,7,7,7,7,7 }};
            std::array<byte_t, 9>  res = {{ 4,6,7,0,0,0,4,6,7 }};

            buf_t inb = { 21, msg.data() }, outb = { 9, out.data() };

            errc_t err = rs_decode(inb, 11, outb);
            
            Assert::AreEqual(errc_t(0), err, L"", LINE_INFO());
            Assert::IsTrue(out == res, L"", LINE_INFO());
        }
		
		TEST_METHOD(rs_decode_alloc)
        {
            std::array<byte_t, 21> msg = {{ 5,2,5,3,3,2,4,0,0,0,0,0,0,0,5,2,5,3,3,2,4 }};
            std::array<byte_t, 9>  res = {{ 4,6,7,0,0,0,4,6,7 }};

            buf_t inb = { 21, msg.data() }, outb = { 0, nullptr };

            __try
            {
                errc_t err = rs_decode(inb, 11, outb);
            
                Assert::AreEqual(errc_t(0), err, L"", LINE_INFO());
                Assert::AreEqual(9, outb.len, L"", LINE_INFO());
                Assert::AreNotEqual(0, outb.len, L"", LINE_INFO());

                Assert::IsTrue(std::equal(res.data(), res.data() + 9, outb.buf), L"", LINE_INFO());
            }
            __finally
            {
                rs_free(outb.buf);
            }
        }
		
		TEST_METHOD(rs_decode_insuff)
        {
            std::array<byte_t, 9> msg = {{ 4,6,7,0,0,0,4,6,7 }};

            buf_t inb = { 21, msg.data() }, outb = { 1, msg.data() };

            errc_t err = rs_decode(inb, 11, outb);

            Assert::AreEqual(errc_t(2), err, L"", LINE_INFO());
        }
    };
}
