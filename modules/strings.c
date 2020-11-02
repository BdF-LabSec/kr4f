/*	Benjamin DELPY `gentilkiwi`
	LabSec - DGSI DIT ARCOS
	benjamin.delpy@banque-france.fr / benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "strings.h"

void printhex(LPCBYTE data, ULONG cbData)
{
	while (cbData--)
	{
		kprintf(L"%02x", *(data++));
	}
}

void printhexData(LPCWSTR prefix, LPCBYTE data, ULONG cbData)
{
	kprintf(L"%s", prefix);
	printhex(data, cbData);
	kprintf(L"\n");
}

BOOLEAN stringToHex(LPCWSTR string, unsigned char* hex, ULONG size)
{
	ULONG i, j;
	BOOLEAN result = (wcslen(string) == ((size_t)size * 2));

	if (result)
	{
		for (i = 0; i < size; i++)
		{
			swscanf_s(&string[i * 2], L"%02x", &j);
			hex[i] = (unsigned char)j;
		}
	}
	return result;
}

PSTR unicode_to_ansi(LPCWSTR unicode)
{
	int needed;
	PSTR buffer = NULL;

	needed = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, unicode, -1, NULL, 0, NULL, NULL);
	if (needed)
	{
		buffer = (char*)LocalAlloc(LPTR, needed);
		if (buffer)
		{
			if (needed != WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, unicode, -1, buffer, needed, NULL, NULL))
			{
				buffer = (char*)LocalFree(buffer);
			}
		}
	}

	return buffer;
}

BOOLEAN SetClip(LPCBYTE Binary, DWORD cbBinary)
{
	BOOLEAN status = FALSE;
	DWORD i;
	LPWSTR szChallenge;
	HANDLE hClip;

	szChallenge = LocalAlloc(LPTR, (cbBinary * 2 + 1) * sizeof(wchar_t));
	if (szChallenge)
	{
		for (i = 0; i < cbBinary; i++)
		{
			swprintf_s(szChallenge + (i * 2), (cbBinary * 2 + 1) - i, L"%02x", Binary[i]);
		}

		if (OpenClipboard(NULL))
		{
			if (EmptyClipboard())
			{
				if (OpenClipboard(NULL))
				{
					hClip = SetClipboardData(CF_UNICODETEXT, szChallenge);
					if (hClip)
					{
						status = TRUE;
					}
					else PRINT_ERROR_AUTO(L"SetClipboardData");
					CloseClipboard();
				}
				else PRINT_ERROR_AUTO(L"OpenClipboard(set)");
			}
			else CloseClipboard();
		}
		else PRINT_ERROR_AUTO(L"OpenClipboard(empty)");

		LocalFree(szChallenge);
	}

	return status;
}

void PressAnyKey()
{
	wint_t keyCode;

	kprintf(L"\nAppuyez sur une touche pour continuer..\n");
	keyCode = _getwch();
}