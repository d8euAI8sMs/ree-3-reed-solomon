#include "stdafx.h"
#include "CppUnitTest.h"

#include <reed-solomon/rs.h>
#include <reed-solomon/m22.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace reedsolomontest
{		
	TEST_CLASS(rs_test)
	{
	public:
		
		TEST_METHOD(blockstream_get_case1)
		{
            gf28::gfinit(11);

            std::array<byte_t, 3 * 3> buf = {{ 1, 2, 3, 4, 5, 6, 7, 8, 0 }};
            gf28::gfpoly_t in1 = {{ 7,7,7,7,7,7,7 }};
            gf28::gfpoly_t in2 = {{ 7,7,7,7,7,7,7 }};
            gf28::gfpoly_t in3 = {{ 7,7,7,7,7,7,7 }};
            gf28::gfpoly_t ex1 = {{ 1,2,3,7,7,7,7 }};
            gf28::gfpoly_t ex2 = {{ 4,5,6,7,7,7,7 }};
            gf28::gfpoly_t ex3 = {{ 7,8,0,7,7,7,7 }};

            size_t res1, res2, res3;

            rs::blockstream s({ 8, buf.data() }, 8, 3);

            res1 = s.get(in1);
            res2 = s.get(in2);
            res3 = s.get(in3);

            Assert::AreEqual(size_t(3), res1, L"", LINE_INFO());
            Assert::AreEqual(size_t(3), res2, L"", LINE_INFO());
            Assert::AreEqual(size_t(2), res3, L"", LINE_INFO());

            Assert::IsTrue(in1 == ex1, L"", LINE_INFO());
            Assert::IsTrue(in2 == ex2, L"", LINE_INFO());
            Assert::IsTrue(in3 == ex3, L"", LINE_INFO());
        }

		TEST_METHOD(blockstream_get_case2)
		{
            gf28::gfinit(11);

            std::array<byte_t, 3> buf = {{ 1, 2, 3 }};
            gf28::gfpoly_t in1 = {{ 7,7,7,7,7,7,7 }};
            gf28::gfpoly_t in2 = {{ 7,7,7,7,7,7,7 }};
            gf28::gfpoly_t in3 = {{ 7,7,7,7,7,7,7 }};
            gf28::gfpoly_t in4 = {{ 7,7,7,7,7,7,7 }};
            gf28::gfpoly_t ex1 = {{ 1,2,3,7,7,7,7 }};
            gf28::gfpoly_t ex2 = {{ 1,2,0,7,7,7,7 }};
            gf28::gfpoly_t ex3 = {{ 1,0,0,7,7,7,7 }};

            size_t res1, res2, res3, res4;

            rs::blockstream s1({ 3, buf.data() }, 8, 3);
            rs::blockstream s2({ 2, buf.data() }, 8, 3);
            rs::blockstream s3({ 1, buf.data() }, 8, 3);
            rs::blockstream s4({ 0, buf.data() }, 8, 3);

            res1 = s1.get(in1);
            res2 = s2.get(in2);
            res3 = s3.get(in3);
            res4 = s4.get(in4);

            Assert::AreEqual(size_t(3), res1, L"", LINE_INFO());
            Assert::AreEqual(size_t(2), res2, L"", LINE_INFO());
            Assert::AreEqual(size_t(1), res3, L"", LINE_INFO());
            Assert::AreEqual(size_t(0), res4, L"", LINE_INFO());

            Assert::IsTrue(in1 == ex1, L"", LINE_INFO());
            Assert::IsTrue(in2 == ex2, L"", LINE_INFO());
            Assert::IsTrue(in3 == ex3, L"", LINE_INFO());
        }
		
		TEST_METHOD(blockstream_get_symbols_case1)
		{
            unsigned int buf =
                (2 << 0)  | (7 << 3)  | (3 << 6)  |
                (4 << 9)  | (1 << 12) | (6 << 15) |
                (2 << 18) | (7 << 21) | (3 << 24) |
                (4 << 27) | (1 << 30);

            gf28::gfpoly_t in1 = {{ 7,7,7,7,7,7,7 }};
            gf28::gfpoly_t in2 = {{ 7,7,7,7,7,7,7 }};
            gf28::gfpoly_t in3 = {{ 7,7,7,7,7,7,7 }};
            gf28::gfpoly_t in4 = {{ 7,7,7,7,7,7,7 }};
            gf28::gfpoly_t ex1 = {{ 2,7,3,7,7,7,7 }};
            gf28::gfpoly_t ex2 = {{ 4,1,6,7,7,7,7 }};
            gf28::gfpoly_t ex3 = {{ 2,7,3,7,7,7,7 }};
            gf28::gfpoly_t ex4 = {{ 4,1,0,7,7,7,7 }};

            size_t res1, res2, res3, res4;

            rs::blockstream s({ 4, (byte_t*)&buf }, 3, 3);

            res1 = s.get(in1);
            res2 = s.get(in2);
            res3 = s.get(in3);
            res4 = s.get(in4);

            Assert::AreEqual(size_t(3), res1, L"", LINE_INFO());
            Assert::AreEqual(size_t(3), res2, L"", LINE_INFO());
            Assert::AreEqual(size_t(3), res3, L"", LINE_INFO());
            Assert::AreEqual(size_t(2), res4, L"", LINE_INFO());

            Assert::IsTrue(in1 == ex1, L"", LINE_INFO());
            Assert::IsTrue(in2 == ex2, L"", LINE_INFO());
            Assert::IsTrue(in3 == ex3, L"", LINE_INFO());
            Assert::IsTrue(in4 == ex4, L"", LINE_INFO());
        }
		
		TEST_METHOD(blockstream_get_symbols_case2)
		{
            unsigned int buf = 0xffffffff;

            gf28::gfpoly_t in = {{ 5,5,5,5,5,5,5 }};
            gf28::gfpoly_t ex;

            size_t res;

            {
                rs::blockstream s({ 1, (byte_t*)&buf }, 3, 3);

                ex = {{ 7,7,3,5,5,5,5 }};
                res = s.get(in);

                Assert::AreEqual(size_t(3), res, L"", LINE_INFO());
                Assert::IsTrue(in == ex, L"", LINE_INFO());
            }
            {
                rs::blockstream s({ 2, (byte_t*)&buf }, 3, 3);

                ex = {{ 7,7,7,5,5,5,5 }};
                res = s.get(in);

                Assert::AreEqual(size_t(3), res, L"", LINE_INFO());
                Assert::IsTrue(in == ex, L"", LINE_INFO());

                ex = {{ 7,7,1,5,5,5,5 }};
                res = s.get(in);

                Assert::AreEqual(size_t(3), res, L"", LINE_INFO());
                Assert::IsTrue(in == ex, L"", LINE_INFO());
            }
            {
                rs::blockstream s({ 3, (byte_t*)&buf }, 3, 3);

                ex = {{ 7,7,7,5,5,5,5 }};
                res = s.get(in);

                Assert::AreEqual(size_t(3), res, L"", LINE_INFO());
                Assert::IsTrue(in == ex, L"", LINE_INFO());

                ex = {{ 7,7,7,5,5,5,5 }};
                res = s.get(in);

                Assert::AreEqual(size_t(3), res, L"", LINE_INFO());
                Assert::IsTrue(in == ex, L"", LINE_INFO());

                ex = {{ 7,7,0,5,5,5,5 }};
                res = s.get(in);

                Assert::AreEqual(size_t(2), res, L"", LINE_INFO());
                Assert::IsTrue(in == ex, L"", LINE_INFO());
            }
            {
                rs::blockstream s({ 4, (byte_t*)&buf }, 3, 3);

                ex = {{ 7,7,7,5,5,5,5 }};
                res = s.get(in);

                Assert::AreEqual(size_t(3), res, L"", LINE_INFO());
                Assert::IsTrue(in == ex, L"", LINE_INFO());

                ex = {{ 7,7,7,5,5,5,5 }};
                res = s.get(in);

                Assert::AreEqual(size_t(3), res, L"", LINE_INFO());
                Assert::IsTrue(in == ex, L"", LINE_INFO());

                ex = {{ 7,7,7,5,5,5,5 }};
                res = s.get(in);

                Assert::AreEqual(size_t(3), res, L"", LINE_INFO());
                Assert::IsTrue(in == ex, L"", LINE_INFO());

                ex = {{ 7,3,0,5,5,5,5 }};
                res = s.get(in);

                Assert::AreEqual(size_t(2), res, L"", LINE_INFO());
                Assert::IsTrue(in == ex, L"", LINE_INFO());
            }
        }
		
		TEST_METHOD(blockstream_put_case1)
		{
            std::array<byte_t, 3 * 3> buf = {{ 7,7,7,7,7,7,7,7,7 }}; // dirty buffer
            std::array<byte_t, 3 * 3> ex = {{ 1,2,3,4,5,6,7,8,7 }};
            gf28::gfpoly_t in1 = {{ 1,2,3,7,7,7,7 }};
            gf28::gfpoly_t in2 = {{ 4,5,6,7,7,7,7 }};
            gf28::gfpoly_t in3 = {{ 7,8,9,7,7,7,7 }};

            size_t res1, res2, res3;

            rs::blockstream s({ 8, buf.data() }, 8, 3);

            res1 = s.put(in1);
            res2 = s.put(in2);
            res3 = s.put(in3);

            Assert::AreEqual(size_t(3), res1, L"", LINE_INFO());
            Assert::AreEqual(size_t(3), res2, L"", LINE_INFO());
            Assert::AreEqual(size_t(2), res3, L"", LINE_INFO());

            Assert::IsTrue(buf == ex, L"", LINE_INFO());
        }
		
		TEST_METHOD(blockstream_put_symbols_case1)
		{
            unsigned int buf = 0;

            gf28::gfpoly_t in;

            size_t res;

            rs::blockstream s({ 2, (byte_t*)&buf }, 3, 3);

            in = {{ 1,2,3,5,5,5,5 }};
            res = s.put(in);

            Assert::AreEqual(size_t(3), res, L"", LINE_INFO());
            Assert::AreEqual((unsigned int)(1), (buf >> 0) & 7, L"", LINE_INFO());
            Assert::AreEqual((unsigned int)(2), (buf >> 3) & 7, L"", LINE_INFO());
            Assert::AreEqual((unsigned int)(3), (buf >> 6) & 7, L"", LINE_INFO());

            in = {{ 4,5,6,5,5,5,5 }};
            res = s.put(in);

            Assert::AreEqual(size_t(3), res, L"", LINE_INFO());
            Assert::AreEqual((unsigned int)(4), (buf >> 9) & 7, L"", LINE_INFO());
            Assert::AreEqual((unsigned int)(5), (buf >> 12) & 7, L"", LINE_INFO());
            Assert::AreEqual((unsigned int)(0), (buf >> 15) & 7, L"", LINE_INFO());
        }
		
		TEST_METHOD(rs2_encoder_encbufsize)
		{
            gf28::gfinit(11);

            Assert::AreEqual(size_t(7), rs::rs2::encblocksize(), L"", LINE_INFO());
            Assert::AreEqual(size_t(3), rs::rs2::decblocksize(), L"", LINE_INFO());
            
            Assert::AreEqual(size_t(7), rs::rs2_encoder::encbufsize(1, false), L"", LINE_INFO());
            Assert::AreEqual(size_t(7), rs::rs2_encoder::encbufsize(2, false), L"", LINE_INFO());
            Assert::AreEqual(size_t(7), rs::rs2_encoder::encbufsize(3, false), L"", LINE_INFO());
            Assert::AreEqual(size_t(14), rs::rs2_encoder::encbufsize(4, false), L"", LINE_INFO());
            Assert::AreEqual(size_t(14), rs::rs2_encoder::encbufsize(5, false), L"", LINE_INFO());
            Assert::AreEqual(size_t(14), rs::rs2_encoder::encbufsize(6, false), L"", LINE_INFO());
            Assert::AreEqual(size_t(21), rs::rs2_encoder::encbufsize(7, false), L"", LINE_INFO());

            gf28::gfinit(25);

            Assert::AreEqual(size_t(360), rs::rs2_encoder::encbufsize(256, false), L"", LINE_INFO());
        }
		
		TEST_METHOD(rs2_encoder_encbufsize_bits)
		{
            gf28::gfinit(11);

            Assert::AreEqual(size_t(7), rs::rs2::encblocksize(), L"", LINE_INFO());
            Assert::AreEqual(size_t(3), rs::rs2::decblocksize(), L"", LINE_INFO());
            
            Assert::AreEqual(size_t(3), rs::rs2_encoder::encbufsize(1, true), L"", LINE_INFO());
            Assert::AreEqual(size_t(6), rs::rs2_encoder::encbufsize(2, true), L"", LINE_INFO());
            Assert::AreEqual(size_t(8), rs::rs2_encoder::encbufsize(3, true), L"", LINE_INFO());
        }
		
		TEST_METHOD(rs2_encoder_decbufsize)
		{
            gf28::gfinit(11);

            Assert::AreEqual(size_t(7), rs::rs2::encblocksize(), L"", LINE_INFO());
            Assert::AreEqual(size_t(3), rs::rs2::decblocksize(), L"", LINE_INFO());
            
            Assert::AreEqual(size_t(3), rs::rs2_encoder::decbufsize(7, false), L"", LINE_INFO());
            Assert::AreEqual(size_t(6), rs::rs2_encoder::decbufsize(14, false), L"", LINE_INFO());
            Assert::AreEqual(size_t(9), rs::rs2_encoder::decbufsize(21, false), L"", LINE_INFO());

            gf28::gfinit(25);

            Assert::AreEqual(size_t(264), rs::rs2_encoder::decbufsize(360, false), L"", LINE_INFO());
        }
		
		TEST_METHOD(rs2_encoder_decbufsize_bits)
		{
            gf28::gfinit(11);

            Assert::AreEqual(size_t(7), rs::rs2::encblocksize(), L"", LINE_INFO());
            Assert::AreEqual(size_t(3), rs::rs2::decblocksize(), L"", LINE_INFO());
            
            Assert::AreEqual(size_t(2), rs::rs2_encoder::decbufsize(3, true), L"", LINE_INFO());
            Assert::AreEqual(size_t(3), rs::rs2_encoder::decbufsize(6, true), L"", LINE_INFO());
            Assert::AreEqual(size_t(4), rs::rs2_encoder::decbufsize(9, true), L"", LINE_INFO());
        }
		
		TEST_METHOD(rs2_encode)
		{
            gf28::gfinit(11);

            gf28::gfpoly_t msg = {{ 4,6,7,0,0,0,0 }};
            gf28::gfpoly_t out = {{ 7,7,7,7,7,7,7 }};
            gf28::gfpoly_t res = {{ 5,2,5,3,3,2,4 }};

            rs::rs2 coder;

            coder.encode(msg, out);

            Assert::IsTrue(res == out, L"", LINE_INFO());
        }
		
		TEST_METHOD(rs2_decode)
		{
            gf28::gfinit(11);

            gf28::gfpoly_t msg = {{ 5,2,5,3,3,2,4 }};
            gf28::gfpoly_t out = {{ 7,7,7,7,7,7,7 }};
            gf28::gfpoly_t res = {{ 4,6,7,0,0,0,0 }};

            rs::rs2 coder;

            coder.decode(msg, out);

            Assert::IsTrue(res == out, L"", LINE_INFO());
        }
		
		TEST_METHOD(rs2_decode_errx2)
		{
            gf28::gfinit(11);
            
            gf28::gfpoly_t err = {{ 0,0,5,0,3,0,0 }};
            gf28::gfpoly_t msg = {{ 5,2,5,3,3,2,4 }};
            gf28::gfpoly_t out = {{ 7,7,7,7,7,7,7 }};
            gf28::gfpoly_t res = {{ 4,6,7,0,0,0,0 }};

            rs::rs2 coder;

            coder.decode(msg + err, out);

            Assert::IsTrue(out == res, L"", LINE_INFO());

            err = {{ 0,0,0,6,0,3,0 }};

            coder.decode(msg + err, out);

            Assert::IsTrue(out == res, L"", LINE_INFO());

            err = {{ 1,2,0,0,0,0,0 }};

            coder.decode(msg + err, out);

            Assert::IsTrue(out == res, L"", LINE_INFO());
        }
		
		TEST_METHOD(rs2_decode_errx1)
		{
            gf28::gfinit(11);
            
            gf28::gfpoly_t err = {{ 0,0,5,0,0,0,0 }};
            gf28::gfpoly_t msg = {{ 5,2,5,3,3,2,4 }};
            gf28::gfpoly_t out = {{ 7,7,7,7,7,7,7 }};
            gf28::gfpoly_t res = {{ 4,6,7,0,0,0,0 }};

            rs::rs2 coder;

            coder.decode(msg + err, out);

            Assert::IsTrue(out == res, L"", LINE_INFO());

            err = {{ 0,0,0,6,0,0,0 }};

            coder.decode(msg + err, out);

            Assert::IsTrue(out == res, L"", LINE_INFO());

            err = {{ 1,0,0,0,0,0,0 }};

            coder.decode(msg + err, out);

            Assert::IsTrue(out == res, L"", LINE_INFO());

            err = {{ 0,0,0,0,0,0,3 }};

            coder.decode(msg + err, out);

            Assert::IsTrue(out == res, L"", LINE_INFO());
        }
		
		TEST_METHOD(rs2_encoder_encode)
        {
            gf28::gfinit(11);

            std::array<byte_t, 9> msg = {{ 4,6,7,0,0,0,4,6,7 }};
            std::array<byte_t, 21> out = {{ 7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7 }};
            std::array<byte_t, 21> res = {{ 5,2,5,3,3,2,4,0,0,0,0,0,0,0,5,2,5,3,3,2,4 }};

            rs::rs2_encoder coder;

            coder.encode({ 9, msg.data() }, { 21, out.data() });

            Assert::IsTrue(out == res, L"", LINE_INFO());
        }
		
		TEST_METHOD(rs2_encoder_decode)
        {
            gf28::gfinit(11);
            
            std::array<byte_t, 21> msg = {{ 5,2,5,3,3,2,4,0,0,0,0,0,0,0,5,2,5,3,3,2,4 }};
            std::array<byte_t, 9> out = {{ 7,7,7,7,7,7,7,7,7 }};
            std::array<byte_t, 9> res = {{ 4,6,7,0,0,0,4,6,7 }};

            rs::rs2_encoder coder;

            coder.decode({ 21, msg.data() }, { 9, out.data() });

            Assert::IsTrue(out == res, L"", LINE_INFO());
        }
    };
}
