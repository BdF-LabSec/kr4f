/*	Benjamin DELPY `gentilkiwi`
	LabSec - DGSI DIT ARCOS
	benjamin.delpy@banque-france.fr / benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "kichal.h"

int wmain(int argc, wchar_t *argv[])
{
	BYTE DerivedKey[24], Challenge[8], Response[sizeof(Challenge)];
	BOOLEAN isDerivedKey = FALSE, isChallenge = FALSE;

	kprintf(L"\n"
		L"  .#####.   kichal 0.01 - " TEXT(__DATE__) L" " TEXT(__TIME__) L"\n"
		L" .## ^ ##.  Edition LabSec! - (oe.eo)\n"
		L" ## / \\ ##  /***\n"
		L" ## \\ / ##   DGSI - DIT - ARCOS | LABSEC ( benjamin.delpy@banque-france.fr )\n"
		L" '## v ##'   Benjamin DELPY `gentilkiwi` ( benjamin@gentilkiwi.com )\n"
		L"  '#####'   ***/\n\n"
		L"+-------------------------------------------------------------------------\n");

	if (argc > 1)
	{
		GetPukAndChallengeFromCLI(argc - 1, argv + 1, DerivedKey, sizeof(DerivedKey), &isDerivedKey, Challenge, sizeof(Challenge), &isChallenge);
	}
	else
	{
		GetPukAndChallengeFromConsole(DerivedKey, sizeof(DerivedKey), &isDerivedKey, Challenge, sizeof(Challenge), &isChallenge);
	}

	if (isDerivedKey)
	{
		kprintf(L"+-------------------------------------------------------------------------\n");
		printhexData(L"| Derived Key           : ", DerivedKey, sizeof(DerivedKey));
		if (isChallenge)
		{
			printhexData(L"| Challenge             : ", Challenge, sizeof(Challenge));
			if (ResponseFromKeyAndChallenge(DerivedKey, sizeof(DerivedKey), Challenge, sizeof(Challenge), Response, sizeof(Response)))
			{
				printhexData(L"| Response              : ", Response, sizeof(Response));
				if (SetClip(Response, sizeof(Response)))
				{
					kprintf(L"i ...copie dans le presse papier!\n");
				}
			}
		}
	}
	
	kprintf(L"+-------------------------------------------------------------------------\n");

	if (argc < 2)
	{
		PressAnyKey();
	}

	return 0;
}

void GetPukAndChallengeFromConsole(IN OUT BYTE *DerivedKey, IN DWORD cbDerivedKey, IN OUT PBOOLEAN pisDerivedKey, IN OUT BYTE *Challenge, IN DWORD cbChallenge, IN OUT PBOOLEAN pisChallenge)
{
	wchar_t input[0xff];
	int ret;

	*pisDerivedKey = FALSE;
	*pisChallenge = FALSE;

	kprintf(L"| Code PUK/AdminKey?    : ");
	ret = wscanf_s(L"%s", input, (DWORD)ARRAYSIZE(input));
	if (ret == 1)
	{
		*pisDerivedKey = stringToHex(input, DerivedKey, cbDerivedKey);
		if (!*pisDerivedKey)
		{
			kprintf(L"i Semble etre un code PUK...\n");
			*pisDerivedKey = DerivePUK(input, DerivedKey, cbDerivedKey);
		}

		if (*pisDerivedKey)
		{
			kprintf(L"| Challenge utilisateur?: ");
			ret = wscanf_s(L"%s", input, (DWORD)ARRAYSIZE(input));
			if (ret == 1)
			{
				*pisChallenge = stringToHex(input, Challenge, cbChallenge);
				if (!*pisChallenge)
				{
					PRINT_ERROR(L"stringToHex(challenge)\n");
				}
			}
			else PRINT_ERROR(L"ret(challenge): %i\n", ret);
		}
	}
	else PRINT_ERROR(L"ret(code): %i\n", ret);
}

void GetPukAndChallengeFromCLI(int argc, wchar_t* argv[], IN OUT BYTE* DerivedKey, IN DWORD cbDerivedKey, IN OUT PBOOLEAN pisDerivedKey, IN OUT BYTE* Challenge, IN DWORD cbChallenge, IN OUT PBOOLEAN pisChallenge)
{
	*pisDerivedKey = FALSE;
	*pisChallenge = FALSE;

	if (argc > 0)
	{
		kprintf(L"| Code PUK/AdminKey     : %s\n", argv[0]);
		*pisDerivedKey = stringToHex(argv[0], DerivedKey, cbDerivedKey);
		if (!*pisDerivedKey)
		{
			kprintf(L"i Semble etre un code PUK...\n");
			*pisDerivedKey = DerivePUK(argv[0], DerivedKey, cbDerivedKey);
		}

		if ((argc > 1) && *pisDerivedKey)
		{
			kprintf(L"| Challenge utilisateur : %s\n", argv[1]);
			*pisChallenge = stringToHex(argv[1], Challenge, cbChallenge);
			if (!*pisChallenge)
			{
				PRINT_ERROR(L"stringToHex(challenge)\n");
			}
		}
	}
}