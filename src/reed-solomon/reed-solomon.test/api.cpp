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
            unsigned long long res =
                (5LL << 0)  | (2LL << 3)  | (5LL << 6)  |
                (3LL << 9)  | (3LL << 12) | (2LL << 15) |
                (4LL << 18) | (0LL << 21) | (0LL << 24) |
                (0LL << 27) | (0LL << 30) | (0LL << 33) |
                (0LL << 36) | (0LL << 39) | (5LL << 42) |
                (2LL << 45) | (5LL << 48) | (3LL << 51) |
                (3LL << 54) | (2LL << 57) | (4LL << 60);
            byte_t out[11];
            unsigned long long outl;
            unsigned long long msg =
                (4LL << 0)  | (6LL << 3)  | (7LL << 6)  |
                (0LL << 9)  | (0LL << 12) | (0LL << 15) |
                (4LL << 18) | (6LL << 21) | (7LL << 24);

            buf_t inb = { 4, (byte_t*)&msg }, outb = { 11, out };

            errc_t err = rs_encode(inb, 11, outb);
            
            Assert::AreEqual(errc_t(0), err, L"", LINE_INFO());

            outl = (*((unsigned long long *)out));

            Assert::IsTrue(outl == res, L"", LINE_INFO());

            Assert::AreEqual(byte_t(0), out[10], L"", LINE_INFO());
            Assert::AreEqual(byte_t(0), out[9], L"", LINE_INFO());
            Assert::AreEqual(byte_t(0), out[8], L"", LINE_INFO());
        }
		
		TEST_METHOD(rs_encode_alloc)
        {
            unsigned long long res =
                (5LL << 0)  | (2LL << 3)  | (5LL << 6)  |
                (3LL << 9)  | (3LL << 12) | (2LL << 15) |
                (4LL << 18) | (0LL << 21) | (0LL << 24) |
                (0LL << 27) | (0LL << 30) | (0LL << 33) |
                (0LL << 36) | (0LL << 39) | (5LL << 42) |
                (2LL << 45) | (5LL << 48) | (3LL << 51) |
                (3LL << 54) | (2LL << 57) | (4LL << 60);
            unsigned long long out = 0;
            unsigned long long msg =
                (4LL << 0)  | (6LL << 3)  | (7LL << 6)  |
                (0LL << 9)  | (0LL << 12) | (0LL << 15) |
                (4LL << 18) | (6LL << 21) | (7LL << 24);

            buf_t inb = { 4, (byte_t*)&msg }, outb = { 0, nullptr };

            __try
            {
                errc_t err = rs_encode(inb, 11, outb);
            
                Assert::AreEqual(errc_t(0), err, L"", LINE_INFO());
                Assert::AreEqual(11, outb.len, L"", LINE_INFO());
                Assert::AreNotEqual(0, outb.len, L"", LINE_INFO());

                out = (*((unsigned long long *)outb.buf));

                Assert::IsTrue(out == res, L"", LINE_INFO());

                Assert::AreEqual(byte_t(0), outb.buf[10], L"", LINE_INFO());
                Assert::AreEqual(byte_t(0), outb.buf[9], L"", LINE_INFO());
                Assert::AreEqual(byte_t(0), outb.buf[8], L"", LINE_INFO());
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
            unsigned long long msg =
                (5LL << 0)  | (2LL << 3)  | (5LL << 6)  |
                (3LL << 9)  | (3LL << 12) | (2LL << 15) |
                (4LL << 18) | (0LL << 21) | (0LL << 24) |
                (0LL << 27) | (0LL << 30) | (0LL << 33) |
                (0LL << 36) | (0LL << 39) | (5LL << 42) |
                (2LL << 45) | (5LL << 48) | (3LL << 51) |
                (3LL << 54) | (2LL << 57) | (4LL << 60);
            unsigned long long out = 0;
            unsigned long long res =
                (4LL << 0)  | (6LL << 3)  | (7LL << 6)  |
                (0LL << 9)  | (0LL << 12) | (0LL << 15) |
                (4LL << 18) | (6LL << 21) | (7LL << 24);

            buf_t inb = { 8, (byte_t*)&msg }, outb = { 8, (byte_t*)&out };

            errc_t err = rs_decode(inb, 11, outb);
            
            Assert::AreEqual(errc_t(0), err, L"", LINE_INFO());
            Assert::IsTrue(out == res, L"", LINE_INFO());
        }
		
		TEST_METHOD(rs_decode_alloc)
        {
            unsigned long long msg =
                (5LL << 0)  | (2LL << 3)  | (5LL << 6)  |
                (3LL << 9)  | (3LL << 12) | (2LL << 15) |
                (4LL << 18) | (0LL << 21) | (0LL << 24) |
                (0LL << 27) | (0LL << 30) | (0LL << 33) |
                (0LL << 36) | (0LL << 39) | (5LL << 42) |
                (2LL << 45) | (5LL << 48) | (3LL << 51) |
                (3LL << 54) | (2LL << 57) | (4LL << 60);
            unsigned long long out = 0;
            unsigned long long res =
                (4LL << 0)  | (6LL << 3)  | (7LL << 6)  |
                (0LL << 9)  | (0LL << 12) | (0LL << 15) |
                (4LL << 18) | (6LL << 21) | (7LL << 24);

            buf_t inb = { 8, (byte_t*)&msg }, outb = { 0, nullptr };

            __try
            {
                errc_t err = rs_decode(inb, 11, outb);
            
                Assert::AreEqual(errc_t(0), err, L"", LINE_INFO());
                Assert::AreEqual(4, outb.len, L"", LINE_INFO());
                Assert::AreNotEqual(0, outb.len, L"", LINE_INFO());

                out = (*((unsigned long long *)outb.buf)) & 0xffffffff;

                Assert::IsTrue(out == res, L"", LINE_INFO());
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
		
		TEST_METHOD(rs_noise_zero)
        {
            std::array<byte_t, 100> msg = {}, dup;

            for (size_t i = 0; i < 0; ++i) msg[i] = (byte_t) i;

            dup = msg;

            for (size_t i = 0; i < 10; ++i)
            {
                rs_noise({ 100, msg.data() }, 0);
            }

            Assert::IsTrue(dup == msg, L"", LINE_INFO());
        }
		
		TEST_METHOD(rs_noise_one)
        {
            std::array<byte_t, 100> msg = {}, dup;

            for (size_t i = 0; i < 0; ++i) msg[i] = (byte_t) i;

            dup = msg;

            rs_noise({ 100, msg.data() }, 1);

            size_t distinct = 0;
            for (size_t i = 0; i < 100; ++i)
            {
                if (msg[i] != dup[i]) ++distinct;
            }

            // don't use (distinct == 100) to be sure test will pass
            Assert::IsTrue(distinct > 80, L"", LINE_INFO());
        }
		
		TEST_METHOD(rs_noise_half)
        {
            std::array<byte_t, 100> msg = {}, dup;

            for (size_t i = 0; i < 0; ++i) msg[i] = (byte_t) i;

            dup = msg;

            rs_noise({ 100, msg.data() }, 0.5);

            size_t distinct = 0;
            for (size_t i = 0; i < 100; ++i)
            {
                if (msg[i] != dup[i]) ++distinct;
            }
            
            // don't use (50 - x < distinct < 50 + x) to be sure test will pass
            Assert::IsTrue(distinct > 25, L"", LINE_INFO());
        }
		
		TEST_METHOD(rs_integrational)
        {
            rs_integrational_n(11, 0, L"11");
            rs_integrational_n(11, 1, L"11");
            rs_integrational_n(11, 2, L"11");

            rs_integrational_n(13, 0, L"13");
            rs_integrational_n(13, 1, L"13");
            rs_integrational_n(13, 2, L"13");

            rs_integrational_n(25, 0, L"25");
            rs_integrational_n(25, 1, L"25");
            rs_integrational_n(25, 2, L"25");

            rs_integrational_n(61, 0, L"61");
            rs_integrational_n(61, 1, L"61");
            rs_integrational_n(61, 2, L"61");
        }

    private:

        void rs_integrational_n(word_t gv, size_t ec, const wchar_t * m)
        {
            std::array<byte_t, 256> msg;

            for (size_t i = 0; i < 256; ++i) msg[i] = rand() % 8;

            buf_t out = { 0, nullptr }, dec = { 0, nullptr };

            rs_encode({ 256, msg.data() }, gv, out);

            if (ec >= 1) out.buf[rand() % 8] = rand() % 8;
            if (ec >= 2) out.buf[rand() % 8] = rand() % 8;

            rs_decode(out, gv, dec);

            __try
            {
                Assert::IsTrue(std::equal(msg.data(), msg.data() + 256, dec.buf), m, LINE_INFO());
            }
            __finally
            {
                rs_free(out.buf); rs_free(dec.buf);
            }
        }
    };
}
