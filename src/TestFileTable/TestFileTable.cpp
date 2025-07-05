#include <sdw.h>

int UMain(int argc, UChar* argv[])
{
	// TestFileTable 0x200000 "Star Ocean - Till the End of Time - Director's Cut (Japan) (Disc 1).iso" disc1d.bin
	// TestFileTable 0 disc1d.bin disc1e.bin
	if (argc != 4)
	{
		return 1;
	}
	n64 nOffset = 0;
	if (StartWith<UString>(argv[1], USTR("0x")) || StartWith<UString>(argv[1], USTR("0X")))
	{
		nOffset = SToN64(argv[1] + 2, 16);
	}
	else
	{
		nOffset = SToN64(argv[1]);
	}
	const n32 nFileCount = 0x1800;
	const n32 nPart = 3;
	FILE* fp = UFopen(argv[2], USTR("rb"), false);
	if (fp == nullptr)
	{
		return 1;
	}
	Fseek(fp, 0, SEEK_END);
	n64 nFileSize = Ftell(fp);
	if (nFileSize < nOffset + nFileCount * nPart * 4)
	{
		fclose(fp);
		return 1;
	}
	u32* pData = new u32[nFileCount * nPart];
	Fseek(fp, nOffset, SEEK_SET);
	fread(pData, 4, nFileCount * nPart, fp);
	fclose(fp);
	if (nOffset == 0)
	{
		pData[0] = 0x34829314;
	}
	const u32 uKey0 = 0x13578642;
	u32 uKey[3] = {};
	for (n32 i = 0; i < nFileCount; i++)
	{
		uKey[0] = uKey[2] ^ (uKey[2] << 2) ^ uKey0;
		uKey[1] = uKey[0] ^ (uKey[0] << 1);
		uKey[2] = ~uKey[1] ^ uKey0;
		for (n32 j = 0; j < nPart; j++)
		{
			pData[nFileCount * j + i] ^= uKey[j];
		}
	}
	if (nOffset != 0)
	{
		pData[0] = 0x400;
	}
	fp = UFopen(argv[3], USTR("wb"), false);
	if (fp == nullptr)
	{
		delete[] pData;
		return 1;
	}
	fwrite(pData, 4, nFileCount * nPart, fp);
	delete[] pData;
	fclose(fp);
	return 0;
}
