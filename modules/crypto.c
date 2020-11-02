/*	Benjamin DELPY `gentilkiwi`
	LabSec - DGSI DIT ARCOS
	benjamin.delpy@banque-france.fr / benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "crypto.h"

const BYTE DerivePUK_Salt[] = { 0x77, 0x1f, 0x5b, 0x30, 0x2c, 0xf7, 0xc5, 0x31, 0xa9, 0x86, 0x46, 0x52, 0xe2, 0xff, 0x4a, 0x17, 0xab, 0xd0, 0x02, 0xdd, 0x4f, 0xb0, 0x2f, 0x71, 0x0f, 0xe5, 0xa8, 0x1a, 0xfe, 0xe7, 0x9c, 0x6b }; // 771f5b302cf7c531a9864652e2ff4a17abd002dd4fb02f710fe5a81afee79c6b
BOOLEAN DerivePUK(IN LPCWCHAR PUK, IN OUT BYTE* DerivedKey, IN DWORD cbDerivedKey)
{
	BOOLEAN status = FALSE;
	NTSTATUS bStatus;
	char* aPuk;

	aPuk = unicode_to_ansi(PUK);
	if (aPuk)
	{
		bStatus = BCryptDeriveKeyPBKDF2(BCRYPT_HMAC_SHA256_ALG_HANDLE, (PUCHAR)aPuk, lstrlenA(aPuk), (PUCHAR)DerivePUK_Salt, sizeof(DerivePUK_Salt), 10003, DerivedKey, cbDerivedKey, 0);
		status = BCRYPT_SUCCESS(bStatus);
		if (!status)
		{
			PRINT_ERROR(L"BCryptDeriveKeyPBKDF2: 0x%08x\n", bStatus);
		}
		LocalFree(aPuk);
	}
	else PRINT_ERROR_AUTO(L"unicode_to_ansi");

	return status;
}

BOOLEAN ResponseFromKeyAndChallenge(IN LPCBYTE Key, IN DWORD cbKey, IN LPCBYTE Challenge, IN DWORD cbChallenge, IN OUT PBYTE Response, IN DWORD cbResponse)
{
	BOOLEAN status = FALSE;
	NTSTATUS bStatus;
	BCRYPT_KEY_HANDLE hKey;
	DWORD cbResult;

	bStatus = BCryptGenerateSymmetricKey(BCRYPT_3DES_CBC_ALG_HANDLE, &hKey, NULL, 0, (PUCHAR)Key, cbKey, 0);
	if (BCRYPT_SUCCESS(bStatus))
	{
		bStatus = BCryptEncrypt(hKey, (PUCHAR)Challenge, cbChallenge, NULL, NULL, 0, Response, cbResponse, &cbResult, 0);
		if (BCRYPT_SUCCESS(bStatus))
		{
			status = (cbResult == cbResponse);
			if (!status)
			{
				PRINT_ERROR(L"cbResult: %u\n", cbResult);
			}
		}
		else PRINT_ERROR(L"BCryptEncrypt: 0x%08x\n", bStatus);
		BCryptDestroyKey(hKey);
	}
	else PRINT_ERROR(L"BCryptGenerateSymmetricKey: 0x%08x\n", bStatus);

	return status;
}