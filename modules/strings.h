/*	Benjamin DELPY `gentilkiwi`
	LabSec - DGSI DIT ARCOS
	benjamin.delpy@banque-france.fr / benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#pragma once
#include <stdio.h>
#include <windows.h>
#include <conio.h>

#if !defined(kprintf)
#define kprintf wprintf
#endif

#if !defined(PRINT_ERROR)
#define PRINT_ERROR(...) (kprintf(L"ERROR " TEXT(__FUNCTION__) L" ; " __VA_ARGS__))
#endif

#if !defined(PRINT_ERROR_AUTO)
#define PRINT_ERROR_AUTO(func) (kprintf(L"ERROR " TEXT(__FUNCTION__) L" ; " func L" (0x%08x)\n", GetLastError()))
#endif

void printhex(LPCBYTE data, ULONG cbData);
void printhexData(LPCWSTR prefix, LPCBYTE data, ULONG cbData);
BOOLEAN stringToHex(LPCWSTR string, unsigned char* hex, ULONG size);
PSTR unicode_to_ansi(LPCWSTR unicode);
BOOLEAN SetClip(LPCBYTE Binary, DWORD cbBinary);
void PressAnyKey();