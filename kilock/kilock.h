/*	Benjamin DELPY `gentilkiwi`
	LabSec - DGSI DIT ARCOS
	benjamin.delpy@banque-france.fr / benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#pragma once
#include <windows.h>
#include "../modules/strings.h"
#include "../modules/crypto.h"

typedef struct _KILOCK_SCARD_DATA {
	SCARDCONTEXT hContext;
	SCARDHANDLE hCard;
	DWORD dwSleep;
	BOOLEAN bMustContinue;
} KILOCK_SCARD_DATA, * PKILOCK_SCARD_DATA;

int wmain(int argc, wchar_t* argv[]);
DWORD WINAPI KeepAliveThread(LPVOID lpThreadParameter);
BOOLEAN GetResponseFromChallengeAndConsole(IN LPCBYTE Challenge, IN DWORD cbChallenge, IN OUT LPBYTE Response, IN DWORD cbResponse);
BOOLEAN GetResponseFromChallengeAndCLI(int argc, wchar_t* argv[], IN LPCBYTE Challenge, IN DWORD cbChallenge, IN OUT LPBYTE Response, IN DWORD cbResponse);
SCARDHANDLE ListReadersAndGetWithACard(SCARDCONTEXT hContext);
BOOLEAN SendRecvISO(SCARDHANDLE hCard, LPCBYTE pbSendBuffer, DWORD cbSendBuffer, LPBYTE pbRecvBuffer, DWORD cbRecvBuffer, PDWORD pcbReceived);