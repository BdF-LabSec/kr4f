/*	Benjamin DELPY `gentilkiwi`
	LabSec - DGSI DIT ARCOS
	benjamin.delpy@banque-france.fr / benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#pragma once
#include <windows.h>
#include "../modules/strings.h"
#include "../modules/crypto.h"

int wmain(int argc, wchar_t* argv[]);
void GetPukAndChallengeFromConsole(IN OUT BYTE* DerivedKey, IN DWORD cbDerivedKey, IN OUT PBOOLEAN pisDerivedKey, IN OUT BYTE* Challenge, IN DWORD cbChallenge, IN OUT PBOOLEAN pisChallenge);
void GetPukAndChallengeFromCLI(int argc, wchar_t* argv[], IN OUT BYTE* DerivedKey, IN DWORD cbDerivedKey, IN OUT PBOOLEAN pisDerivedKey, IN OUT BYTE* Challenge, IN DWORD cbChallenge, IN OUT PBOOLEAN pisChallenge);