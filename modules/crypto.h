/*	Benjamin DELPY `gentilkiwi`
	LabSec - DGSI DIT ARCOS
	benjamin.delpy@banque-france.fr / benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#pragma once
#include <windows.h>
#include "strings.h"

BOOLEAN DerivePUK(IN LPCWCHAR PUK, IN OUT BYTE* DerivedKey, IN DWORD cbDerivedKey);
BOOLEAN ResponseFromKeyAndChallenge(IN LPCBYTE Key, IN DWORD cbKey, IN LPCBYTE Challenge, IN DWORD cbChallenge, IN OUT PBYTE Response, IN DWORD cbResponse);