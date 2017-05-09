#include "stdafx.h"
#include "CppUnitTest.h"

#include "../CumBuffer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


const uint64_t UINT64_ZERO = 0;

namespace VSUnitTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(AppendAndGet)
		{
			char data[100] = { 0, };
			char dataOut[100] = { 0, };

			CumBuffer buffering;
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Init()); //default buffer length
			Assert::AreEqual(buffering.GetCurHeadPos(), UINT64_ZERO);
			Assert::AreEqual(buffering.GetCurTailPos(), UINT64_ZERO);
			Assert::IsTrue(OP_RESULT::OP_RSLT_NO_DATA == buffering.GetData(3, dataOut));

			memset(data, 0x00, sizeof(data));
			memcpy(data, (void*)"aaa", 3);
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Append(3, data));
			Assert::AreEqual(buffering.GetCurTailPos(), (uint64_t)3);

			memset(data, 0x00, sizeof(data));
			memcpy(data, (void*)"bbb", 3);
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Append(3, data));
			Assert::AreEqual(buffering.GetCurTailPos(), (uint64_t)6);
			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)6);

			memset(dataOut, 0x00, sizeof(dataOut));
			Assert::IsTrue(OP_RESULT::OP_RSLT_INVALID_LEN == buffering.GetData(7, dataOut));
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.GetData(6, dataOut));
			Assert::AreEqual("aaabbb", dataOut);
			Assert::AreEqual(buffering.GetCurHeadPos(), (uint64_t)6);
			Assert::AreEqual(buffering.GetCurTailPos(), (uint64_t)6);
		}

		TEST_METHOD(AppendAndGetLoop)
		{
			char data[100];
			char dataOut[100];

			CumBuffer buffering;
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Init(9));
			Assert::AreEqual(buffering.GetCurHeadPos(), (uint64_t)0);
			Assert::AreEqual(buffering.GetCurTailPos(), (uint64_t)0);

			for (int i = 0; i < 30; ++i)
			{
				memset(data, 0x00, sizeof(data));
				memcpy(data, (void*)"aaa", 3);//준비
				Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Append(3, data));//실행,체크

				memset(data, 0x00, sizeof(data));
				memcpy(data, (void*)"abbb", 4);
				Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Append(4, data));
				Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)7);

				memset(dataOut, 0x00, sizeof(dataOut));
				Assert::IsTrue(OP_RESULT::OP_RSLT_INVALID_LEN == buffering.GetData(8, dataOut));
				Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.GetData(4, dataOut));
				Assert::AreEqual("aaaa", dataOut);

				memset(dataOut, 0x00, sizeof(dataOut));
				Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.GetData(3, dataOut));
				Assert::AreEqual("bbb", dataOut);

			}
		}

		TEST_METHOD(LinearTest1)
		{

			char data[100];
			char dataOut[100];

			CumBuffer buffering;
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Init(10));
			Assert::AreEqual(buffering.GetCapacity(), (uint64_t)10);
			Assert::AreEqual(buffering.GetCurHeadPos(), (uint64_t)0);
			Assert::AreEqual(buffering.GetCurTailPos(), (uint64_t)0);
			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)0);
			Assert::AreEqual(buffering.GetLinearFreeSpace(), (uint64_t)10);

			memset(data, 0x00, sizeof(data));
			memcpy(data, (void*)"a.....b", 7);
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Append(7, data));
			Assert::AreEqual(buffering.GetCurHeadPos(), (uint64_t)0);
			Assert::AreEqual(buffering.GetCurTailPos(), (uint64_t)7);
			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)7);
			Assert::AreEqual(buffering.GetLinearFreeSpace(), (uint64_t)3);

			memset(dataOut, 0x00, sizeof(dataOut));
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.GetData(4, dataOut));
			Assert::AreEqual("a...", dataOut);
			Assert::AreEqual(buffering.GetCurHeadPos(), (uint64_t)4);
			Assert::AreEqual(buffering.GetCurTailPos(), (uint64_t)7);
			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)3);
			Assert::AreEqual(buffering.GetTotalFreeSpace(), (uint64_t)7);
			Assert::AreEqual(buffering.GetLinearFreeSpace(), (uint64_t)3);

			memset(data, 0x00, sizeof(data));
			memcpy(data, (void*)"c***d", 5);
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Append(5, data));
			Assert::AreEqual(buffering.GetCurHeadPos(), (uint64_t)4);
			Assert::AreEqual(buffering.GetCurTailPos(), (uint64_t)2);
			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)8);
			Assert::AreEqual(buffering.GetTotalFreeSpace(), (uint64_t)2);
			Assert::AreEqual(buffering.GetLinearFreeSpace(), (uint64_t)2);

			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.PeekData(8, dataOut));
			Assert::AreEqual((uint64_t)strncmp("..bc***d", dataOut, 8), (uint64_t)0);


		}

		TEST_METHOD(LinearTest2)
		{
			char data[100];
			char dataOut[100];

			CumBuffer buffering;
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Init(10));
			Assert::AreEqual(buffering.GetTotalFreeSpace(), (uint64_t)10);
			Assert::AreEqual(buffering.GetLinearFreeSpace(), (uint64_t)10);

			memcpy(data, (void*)"abcde", 5);
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Append(5, data));
			Assert::AreEqual(buffering.GetTotalFreeSpace(), (uint64_t)5);
			Assert::AreEqual(buffering.GetLinearFreeSpace(), (uint64_t)5);

			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.PeekData(5, dataOut));
			Assert::AreEqual((uint64_t)strncmp("abcde", dataOut, 5), (uint64_t)0);
			Assert::AreEqual(buffering.GetTotalFreeSpace(), (uint64_t)5);
			Assert::AreEqual(buffering.GetLinearFreeSpace(), (uint64_t)5);

			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.ConsumeData(2));
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.PeekData(3, dataOut));
			Assert::AreEqual(strncmp("cde", dataOut, 3), 0);
			Assert::AreEqual(buffering.GetTotalFreeSpace(), (uint64_t)7);
			Assert::AreEqual(buffering.GetLinearFreeSpace(), (uint64_t)5);
			
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.ConsumeData(2));
			Assert::AreEqual(buffering.GetTotalFreeSpace(), (uint64_t)9);
			Assert::AreEqual(buffering.GetLinearFreeSpace(), (uint64_t)5);
		}

		TEST_METHOD(LinearTest3)
		{
			char data[100];
			char dataOut[100];

			CumBuffer buffering;
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Init(10));
			Assert::AreEqual(buffering.GetCapacity(), (uint64_t)10);
			Assert::AreEqual(buffering.GetCurHeadPos(), (uint64_t)0);
			Assert::AreEqual(buffering.GetCurTailPos(), (uint64_t)0);
			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)0);
			Assert::AreEqual(buffering.GetLinearFreeSpace(), (uint64_t)10);

			memset(data, 0x00, sizeof(data));
			memcpy(data, (void*)"aaa", 3);
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Append(3, data));
			Assert::AreEqual(buffering.GetCurHeadPos(), (uint64_t)0);
			Assert::AreEqual(buffering.GetCurTailPos(), (uint64_t)3);
			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)3);
			Assert::AreEqual(buffering.GetLinearFreeSpace(), (uint64_t)7);

			memset(data, 0x00, sizeof(data));
			memcpy(data, (void*)"bbbb", 4);
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Append(4, data));
			Assert::AreEqual(buffering.GetCurHeadPos(), (uint64_t)0);
			Assert::AreEqual(buffering.GetCurTailPos(), (uint64_t)7);
			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)7);
			Assert::AreEqual(buffering.GetLinearFreeSpace(), (uint64_t)3);

			memset(data, 0x00, sizeof(data));
			memcpy(data, (void*)"ccc", 4);
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Append(3, data));
			Assert::AreEqual(buffering.GetCurHeadPos(), (uint64_t)0);
			Assert::AreEqual(buffering.GetCurTailPos(), (uint64_t)10);
			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)10);
			Assert::AreEqual(buffering.GetLinearFreeSpace(), (uint64_t)0);

			memset(dataOut, 0x00, sizeof(dataOut));
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.GetData(5, dataOut));
			Assert::AreEqual("aaabb", dataOut);
			Assert::AreEqual(buffering.GetCurHeadPos(), (uint64_t)5);
			Assert::AreEqual(buffering.GetCurTailPos(), (uint64_t)10);
			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)5);
			Assert::AreEqual(buffering.GetLinearFreeSpace(), (uint64_t)5);

			memset(dataOut, 0x00, sizeof(dataOut));
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.GetData(2, dataOut));
			Assert::AreEqual("bb", dataOut);
			Assert::AreEqual(buffering.GetCurHeadPos(), (uint64_t)7);
			Assert::AreEqual(buffering.GetCurTailPos(), (uint64_t)10);
			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)3);
			Assert::AreEqual(buffering.GetLinearFreeSpace(), (uint64_t)7);

			memset(dataOut, 0x00, sizeof(dataOut));
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.GetData(3, dataOut));
			Assert::AreEqual("ccc", dataOut);
			Assert::AreEqual(buffering.GetCurHeadPos(), (uint64_t)10);
			Assert::AreEqual(buffering.GetCurTailPos(), (uint64_t)10);
			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)0);
			Assert::AreEqual(buffering.GetLinearFreeSpace(), (uint64_t)10);

			memset(data, 0x00, sizeof(data));
			memcpy(data, (void*)"ddd", 3);
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Append(3, data));
			Assert::AreEqual(buffering.GetCurHeadPos(), (uint64_t)10);
			Assert::AreEqual(buffering.GetCurTailPos(), (uint64_t)3);
			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)3);
			Assert::AreEqual(buffering.GetLinearFreeSpace(), (uint64_t)7);

			memset(dataOut, 0x00, sizeof(data));
			memcpy(data, (void*)"eeee", 4);
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Append(4, data));
			Assert::AreEqual(buffering.GetCurHeadPos(), (uint64_t)10);
			Assert::AreEqual(buffering.GetCurTailPos(), (uint64_t)7);
			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)7);
			Assert::AreEqual(buffering.GetLinearFreeSpace(), (uint64_t)3);

			memset(dataOut, 0x00, sizeof(dataOut));
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.GetData(4, dataOut));
			Assert::AreEqual("ddde", dataOut);
			Assert::AreEqual(buffering.GetCurHeadPos(), (uint64_t)4);
			Assert::AreEqual(buffering.GetCurTailPos(), (uint64_t)7);
			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)3);
			Assert::AreEqual(buffering.GetLinearFreeSpace(), (uint64_t)3);

			memset(dataOut, 0x00, sizeof(dataOut));
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.GetData(3, dataOut));
			Assert::AreEqual("eee", dataOut);
			Assert::AreEqual(buffering.GetCurHeadPos(), (uint64_t)7);
			Assert::AreEqual(buffering.GetCurTailPos(), (uint64_t)7);
			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)0);
			Assert::AreEqual(buffering.GetLinearFreeSpace(), (uint64_t)3);

			memset(data, 0x00, sizeof(data));
			memcpy(data, (void*)"fff", 3);
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Append(3, data));
			Assert::AreEqual(buffering.GetCurHeadPos(), (uint64_t)7);
			Assert::AreEqual(buffering.GetCurTailPos(), (uint64_t)10);
			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)3);
			Assert::AreEqual(buffering.GetLinearFreeSpace(), (uint64_t)7);

		}

		TEST_METHOD(LinearTest4)
		{
			char data[100];
			char dataOut[100];

			CumBuffer buffering;
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Init(10));

			memcpy(data, (void*)"abcde", 5);
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Append(5, data));
			memcpy(data, (void*)"ghijk", 5);
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Append(5, data));

			Assert::AreEqual(buffering.GetCurHeadPos(), (uint64_t)0);
			Assert::AreEqual(buffering.GetCurTailPos(), (uint64_t)10);

			memset(dataOut, 0x00, sizeof(dataOut));
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.GetData(10, dataOut));
			Assert::AreEqual("abcdeghijk", dataOut);

			Assert::AreEqual(buffering.GetCurHeadPos(), (uint64_t)10);
			Assert::AreEqual(buffering.GetCurTailPos(), (uint64_t)10);

			Assert::IsTrue(OP_RESULT::OP_RSLT_NO_DATA == buffering.GetData(1, dataOut));

			Assert::AreEqual(buffering.GetTotalFreeSpace(), (uint64_t)10);
			Assert::AreEqual(buffering.GetLinearFreeSpace(), (uint64_t)10);

		}

		TEST_METHOD(IncreaseData)
		{
			char dataOut[100];

			CumBuffer buffering;
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Init(10));
			Assert::AreEqual(buffering.GetTotalFreeSpace(), (uint64_t)10);

			memcpy(buffering.GetLinearAppendPtr(), (void*)"abcde", 5);//준비
			
			buffering.IncreaseData(5);//실행
			Assert::AreEqual(buffering.GetTotalFreeSpace(), (uint64_t)5);

			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.PeekData(5, dataOut));
			Assert::AreEqual(strncmp("abcde", dataOut, 5), 0);//체크
		}

		TEST_METHOD(ConsumeData)
		{
			char data[100];
			char dataOut[100];

			CumBuffer buffering;
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Init(10));
			memcpy(data, (void*)"abcde", 5);
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Append(5, data));

			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.PeekData(5, dataOut));
			Assert::AreEqual(strncmp("abcde", dataOut, 5), 0);

			Assert::IsTrue(OP_RESULT::OP_RSLT_INVALID_LEN == buffering.ConsumeData(11));
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.ConsumeData(2));
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.PeekData(3, dataOut));
			Assert::AreEqual(strncmp("cde", dataOut, 3), 0);
			Assert::AreEqual(buffering.GetCurHeadPos(), (uint64_t)2);
			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)3);
		}

		TEST_METHOD(BufferFull)
		{
			char data[100];
			char dataOut[100];

			CumBuffer buffering;
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Init(10));

			Assert::IsTrue(OP_RESULT::OP_RSLT_INVALID_LEN == buffering.Append(11, data));
			Assert::IsTrue(OP_RESULT::OP_RSLT_INVALID_LEN == buffering.Append(20, data));

			memcpy(data, (void*)"aaaaa", 5);
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Append(5, data));

			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.PeekData(5, dataOut));
			Assert::AreEqual(strncmp("aaaaa", dataOut, 5), 0);

			Assert::IsTrue(OP_RESULT::OP_RSLT_INVALID_LEN == buffering.GetData(10, dataOut));

			memset(data, 0x00, sizeof(data));
			memcpy(data, (void*)"bbbbb", 5);
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Append(5, data));

			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.PeekData(10, dataOut));
			Assert::AreEqual(strncmp("aaaaabbbbb", dataOut, 10), 0);

			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)10);
			Assert::AreEqual(buffering.GetCurTailPos(), (uint64_t)10);
			Assert::IsTrue(OP_RESULT::OP_RSLT_INVALID_LEN == buffering.GetData(11, dataOut));
			Assert::IsTrue(OP_RESULT::OP_RSLT_INVALID_LEN == buffering.GetData(20, dataOut));

			memset(data, 0x00, sizeof(data));
			memcpy(data, (void*)"ccc", 3);
			Assert::IsTrue(OP_RESULT::OP_RSLT_BUFFER_FULL == buffering.Append(3, data));
			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)10);

			memset(dataOut, 0x00, sizeof(dataOut));
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.GetData(3, dataOut));
			Assert::AreEqual("aaa", dataOut);
			Assert::AreEqual(buffering.GetCurHeadPos(), (uint64_t)3);
			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)7);
			Assert::IsTrue(OP_RESULT::OP_RSLT_BUFFER_FULL == buffering.Append(4, data));
			Assert::IsTrue(OP_RESULT::OP_RSLT_INVALID_LEN == buffering.GetData(8, dataOut));

			//---------------------------------------------------------------------------------

			memset(data, 0x00, sizeof(data));
			memcpy(data, (void*)"ccc", 3);
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Append(3, data));

			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.PeekData(10, dataOut));
			Assert::AreEqual(strncmp("aabbbbbccc", dataOut, 10), 0);

			Assert::AreEqual(buffering.GetCurTailPos(), (uint64_t)3);
			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)10);
			Assert::IsTrue(OP_RESULT::OP_RSLT_INVALID_LEN == buffering.GetData(11, dataOut));
			
			memset(data, 0x00, sizeof(data));
			memcpy(data, (void*)"ddd", 3);
			Assert::IsTrue(OP_RESULT::OP_RSLT_BUFFER_FULL == buffering.Append(3, data));
			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)10);

			memset(dataOut, 0x00, sizeof(dataOut));
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.GetData(3, dataOut));
			Assert::AreEqual("aab", dataOut);
			Assert::AreEqual(buffering.GetCurHeadPos(), (uint64_t)6);
			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)7);
			Assert::IsTrue(OP_RESULT::OP_RSLT_INVALID_LEN == buffering.GetData(8, dataOut));
			
			//---------------------------------------------------------------------------------

			memset(data, 0x00, sizeof(data));
			memcpy(data, (void*)"ddd", 3);
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.Append(3, data));
			Assert::AreEqual(buffering.GetCurHeadPos(), (uint64_t)6);
			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)6);

			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.PeekData(10, dataOut));
			Assert::AreEqual(strncmp("bbbbcccddd", dataOut, 10), 0);

			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)10);
			Assert::IsTrue(OP_RESULT::OP_RSLT_INVALID_LEN == buffering.GetData(11, dataOut));

			memset(data, 0x00, sizeof(data));
			memcpy(data, (void*)"eeeeee", 6);
			Assert::IsTrue(OP_RESULT::OP_RSLT_BUFFER_FULL == buffering.Append(6, data));
			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)10);

			memset(dataOut, 0x00, sizeof(dataOut));
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.GetData(6, dataOut));
			Assert::AreEqual("bbbbcc", dataOut);
			Assert::AreEqual(buffering.GetCurHeadPos(), (uint64_t)2);
			Assert::AreEqual(buffering.GetCurTailPos(), (uint64_t)6);
			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)4);
			Assert::IsTrue(OP_RESULT::OP_RSLT_INVALID_LEN == buffering.GetData(5, dataOut));

			memset(data, 0x00, sizeof(data));
			memcpy(data, (void*)"eeeeee", 6);
			Assert::IsTrue(OP_RESULT::OP_RSLT_BUFFER_FULL == buffering.Append(6, data));

			Assert::IsTrue(OP_RESULT::OP_RSLT_BUFFER_FULL == buffering.PeekData(10, dataOut));
			Assert::AreEqual(strncmp("cdddeeeeee", dataOut, 10), 0);

			memset(dataOut, 0x00, sizeof(dataOut));
			Assert::IsTrue(OP_RESULT::OP_RSLT_OK == buffering.GetData(10, dataOut));
			Assert::AreEqual("cdddeeeeee", dataOut);
			Assert::AreEqual(buffering.GetCurHeadPos(), (uint64_t)2);
			Assert::AreEqual(buffering.GetCurTailPos(), (uint64_t)2);
			Assert::AreEqual(buffering.GetCumulatedLen(), (uint64_t)0);

		}

	};
}