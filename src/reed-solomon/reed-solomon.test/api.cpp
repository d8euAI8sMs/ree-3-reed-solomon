#include "stdafx.h"
#include "CppUnitTest.h"

#include <reed-solomon/api.h>
#include <reed-solomon/rs.h>

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
		
		TEST_METHOD(rs_noise_all)
        {
            rs_noise_n(1, 0, 0);
            rs_noise_n(2, 0, 0);
            rs_noise_n(1, 1, 0.8f);
            rs_noise_n(2, 1, 0.8f);
            rs_noise_n(1, 0.5f, 0.25f);
            rs_noise_n(2, 0.5f, 0.25f);
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
		
		void rs_noise_n(size_t ec, float freq, float thres)
        {
            std::array<byte_t, 100> msg = {}, dup;

            for (size_t i = 0; i < 100; ++i) msg[i] = (byte_t) i;

            dup = msg;

            rs_noise({ 100, msg.data() }, 11, ec, freq);

            size_t block = rs::rs2::encblocksize();

            rs::blockstream in1({ 100, msg.data() }, gf28::gfscal_t::gp, block);
            rs::blockstream in2({ 100, dup.data() }, gf28::gfscal_t::gp, block);

            gf28::gfpoly_t buf1, buf2;

            size_t epb = 0, et = 0, bc = 0, read;

            while (read = in1.get(buf1))
            {
                in2.get(buf2);
                
                size_t e = 0;
                for (size_t i = 0; i < read; ++i)
                {
                    if (buf1[i] != buf2[i]) ++e;
                }

                epb = (epb > e ? epb : e);
                et += (e > 0 ? 1 : 0);

                ++bc;
            }

            Assert::IsTrue((float)et / (float)bc >= thres, L"", LINE_INFO());
            Assert::IsTrue(epb <= ec, L"", LINE_INFO());
        }

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
