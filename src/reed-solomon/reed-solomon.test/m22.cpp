#include "stdafx.h"
#include "CppUnitTest.h"

#include <reed-solomon/m22.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace reedsolomontest
{		
	TEST_CLASS(m22_test)
	{
	public:
		
		TEST_METHOD(m22_inv)
        {
            byte_t s0 = 6, s1 = 5, s2 = 5, s3 = 3;
            gf28::m22 m = { s1, s0, s2, s1 }, m2;
            bool r = inv(m, m2);
            auto m3 = m * m2;
            auto m4 = m2 * m;

            Assert::IsTrue(r, L"", LINE_INFO());
            
            Assert::AreEqual(byte_t(1), (byte_t)m3.m11, L"", LINE_INFO());
            Assert::AreEqual(byte_t(0), (byte_t)m3.m12, L"", LINE_INFO());
            Assert::AreEqual(byte_t(0), (byte_t)m3.m21, L"", LINE_INFO());
            Assert::AreEqual(byte_t(1), (byte_t)m3.m22, L"", LINE_INFO());
        
            Assert::AreEqual(byte_t(1), (byte_t)m4.m11, L"", LINE_INFO());
            Assert::AreEqual(byte_t(0), (byte_t)m4.m12, L"", LINE_INFO());
            Assert::AreEqual(byte_t(0), (byte_t)m4.m21, L"", LINE_INFO());
            Assert::AreEqual(byte_t(1), (byte_t)m4.m22, L"", LINE_INFO());
        }
		
		TEST_METHOD(m22_inv_fail)
        {
            gf28::m22 m = { 1, 2, 2, 4 }, m2;
            bool r = inv(m, m2);

            Assert::IsFalse(r, L"", LINE_INFO());
        }
		
		TEST_METHOD(m22_lsol)
        {
            byte_t s0 = 6, s1 = 5, s2 = 5, s3 = 3;
            gf28::m22 m = { s1, s0, s2, s1 };
        
            gf28::v2 v = { s2, s3 }, s;
            bool r = lsol(m, v, s);

            Assert::IsTrue(r, L"", LINE_INFO());
            
            Assert::AreEqual(byte_t(4), (byte_t)s.v1, L"", LINE_INFO());
            Assert::AreEqual(byte_t(2), (byte_t)s.v2, L"", LINE_INFO());
        }
		
		TEST_METHOD(m22_lsol_fail)
        {
            gf28::m22 m = { 1, 2, 2, 4 };
        
            gf28::v2 v = { 7, 7 }, s;
            bool r = lsol(m, v, s);

            Assert::IsFalse(r, L"", LINE_INFO());
        }
    };
}
