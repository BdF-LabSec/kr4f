/*	Benjamin DELPY `gentilkiwi`
	LabSec - DGSI DIT ARCOS
	benjamin.delpy@banque-france.fr / benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "kilock.h"

const BYTE SelectDF[] = { 0x00, 0xa4, 0x04, 0x00, 0x0b, 0xa0, 0x00, 0x00, 0x00, 0x18, 0x80, 0x00, 0x00, 0x00, 0x06, 0x62 };
const BYTE ActivateApplet[] = { 0x00, 0xa6, 0x00, 0x00, 0x15 };
const BYTE GetChallenge[] = { 0x00, 0x84, 0x00, 0x00, 0x08 };
const BYTE ResetPin[] = { 0x00, 0x2c, 0x10, 0x11, 0x10, 0x57, 0x61, 0x7a, 0x61, 0x31, 0x32, 0x33, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // Waza1234

int wmain(int argc, wchar_t* argv[])
{
	LONG scStatus;
	KILOCK_SCARD_DATA data = { 0, 0, 2000, TRUE };
	BYTE RecvBuffer[256], Response[] = { 0x00, 0x82, 0x00, 0x31, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	DWORD cbRecvLength;
	BOOLEAN isResponse = FALSE;

	UNREFERENCED_PARAMETER(argv);

	kprintf(L"\n"
		L"  .#####.   kilock 0.01 - " TEXT(__DATE__) L" " TEXT(__TIME__) L"\n"
		L" .## ^ ##.  Edition LabSec! - (oe.eo)\n"
		L" ## / \\ ##  /***\n"
		L" ## \\ / ##   DGSI - DIT - ARCOS | LABSEC ( benjamin.delpy@banque-france.fr )\n"
		L" '## v ##'   Benjamin DELPY `gentilkiwi` ( benjamin@gentilkiwi.com )\n"
		L"  '#####'   ***/\n\n"
		L"+-------------------------------------------------------------------------\n");

	scStatus = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &data.hContext);
	if (scStatus == SCARD_S_SUCCESS)
	{
		data.hCard = ListReadersAndGetWithACard(data.hContext);
		if (data.hCard)
		{
			kprintf(L"+-------------------------------------------------------------------------\n");
			scStatus = SCardBeginTransaction(data.hCard);
			if (scStatus == SCARD_S_SUCCESS)
			{
				if (CreateThread(NULL, 0, KeepAliveThread, &data, 0, NULL))
				{
					kprintf(L"> Selection de fichier...\n");
					if (SendRecvISO(data.hCard, SelectDF, sizeof(SelectDF), RecvBuffer, sizeof(RecvBuffer), &cbRecvLength))
					{
						kprintf(L"> Reset et activation de l\'applet...\n");
						if (SendRecvISO(data.hCard, ActivateApplet, sizeof(ActivateApplet), RecvBuffer, sizeof(RecvBuffer), &cbRecvLength))
						{
							kprintf(L"> Demande de challenge...\n");
							if (SendRecvISO(data.hCard, GetChallenge, sizeof(GetChallenge), RecvBuffer, sizeof(RecvBuffer), &cbRecvLength))
							{
								if (cbRecvLength == 8)
								{
									kprintf(L"| Challenge: ");
									printhex(RecvBuffer, 8);
									kprintf(L"\n");

									kprintf(L"+-------------------------------------------------------------------------\n");
									if (argc > 1)
									{
										isResponse = GetResponseFromChallengeAndCLI(argc - 1, argv + 1, RecvBuffer, 8, Response + 5, 8);
									}
									else
									{
										isResponse = GetResponseFromChallengeAndConsole(RecvBuffer, 8, Response + 5, 8);
									}
									kprintf(L"+-------------------------------------------------------------------------\n");

									if (isResponse)
									{
										kprintf(L"| Response : ");
										printhex(Response + 5, 8);
										kprintf(L"\n");

										kprintf(L"> Envoi de la reponse...\n");
										if (SendRecvISO(data.hCard, Response, sizeof(Response), RecvBuffer, sizeof(RecvBuffer), &cbRecvLength))
										{
											kprintf(L"> Envoi du nouveau code pin...\n");
											if (SendRecvISO(data.hCard, ResetPin, sizeof(ResetPin), RecvBuffer, sizeof(RecvBuffer), &cbRecvLength))
											{
												kprintf(L"| OK !\n");
											}
										}
									}
								}
								else PRINT_ERROR(L"Taille du challenge incorrecte: %u\n", cbRecvLength);
							}
						}
					}
					data.bMustContinue = FALSE;
				}
				else PRINT_ERROR_AUTO(L"CreateThread");
				SCardEndTransaction(data.hCard, SCARD_LEAVE_CARD);
			}
			else PRINT_ERROR(L"SCardBeginTransaction: 0x%08x\n", scStatus);
			SCardDisconnect(data.hCard, SCARD_LEAVE_CARD);
			data.hCard = 0;
		}
		else PRINT_ERROR(L"Pas de carte\n");
		SCardReleaseContext(data.hContext);
	}
	else if (scStatus == SCARD_E_NO_SERVICE)
	{
		PRINT_ERROR(L"Le service de carte a puce ne s\'execute pas, pas de lecteur/token?\n");
	}
	else PRINT_ERROR(L"SCardEstablishContext: 0x%08x\n", scStatus);

	kprintf(L"+-------------------------------------------------------------------------\n");

	if (argc < 2)
	{
		PressAnyKey();
	}

	return 0;
}

DWORD WINAPI KeepAliveThread(LPVOID lpThreadParameter)
{
	PKILOCK_SCARD_DATA pData = (PKILOCK_SCARD_DATA)lpThreadParameter;

	while (pData->hCard && pData->bMustContinue)
	{
		SCardStatus(pData->hCard, NULL, NULL, NULL, NULL, NULL, NULL);
		Sleep(pData->dwSleep);
	};
	return ERROR_SUCCESS;
}

BOOLEAN GetResponseFromChallengeAndConsole(IN LPCBYTE Challenge, IN DWORD cbChallenge, IN OUT LPBYTE Response, IN DWORD cbResponse)
{
	BOOLEAN status = FALSE;
	wchar_t input[0xff];
	int ret;

	if (SetClip(Challenge, cbChallenge))
	{
		kprintf(L"| ... copie dans le presse papier!\n");
	}

	kprintf(L"| Response?: ");
	ret = wscanf_s(L"%s", input, (DWORD)ARRAYSIZE(input));
	if (ret == 1)
	{
		status = stringToHex(input, Response, cbResponse);
		if (!status)
		{
			PRINT_ERROR(L"stringToHex\n");
		}
	}
	else PRINT_ERROR(L"ret: %i\n", ret);

	return status;
}

BOOLEAN GetResponseFromChallengeAndCLI(int argc, wchar_t* argv[], IN LPCBYTE Challenge, IN DWORD cbChallenge, IN OUT LPBYTE Response, IN DWORD cbResponse)
{
	BOOLEAN status = FALSE, isDerivedKey;
	BYTE DerivedKey[24];

	if (argc > 0)
	{
		kprintf(L"| Code PUK/AdminKey     : %s\n", argv[0]);
		isDerivedKey = stringToHex(argv[0], DerivedKey, sizeof(DerivedKey));
		if (!isDerivedKey)
		{
			kprintf(L"i Semble etre un code PUK...\n");
			isDerivedKey = DerivePUK(argv[0], DerivedKey, sizeof(DerivedKey));
		}

		if (isDerivedKey)
		{
			printhexData(L"| Derived Key           : ", DerivedKey, sizeof(DerivedKey));
			status = ResponseFromKeyAndChallenge(DerivedKey, sizeof(DerivedKey), Challenge, cbChallenge, Response, cbResponse);
		}
	}

	return status;
}

SCARDHANDLE ListReadersAndGetWithACard(SCARDCONTEXT hContext)
{
	SCARDHANDLE hCard = 0;
	PBYTE atr;
	LONG scStatus;
	LPWSTR mszReaders = NULL, pReader, mszCards = NULL, pCard;
	DWORD dwLen = SCARD_AUTOALLOCATE, dwAtrLen;

	scStatus = SCardListReaders(hContext, SCARD_ALL_READERS, (LPWSTR)&mszReaders, &dwLen);
	if (scStatus == SCARD_S_SUCCESS)
	{
		kprintf(L"| Lecteur(s):\n");
		for (pReader = mszReaders; pReader && *pReader && !hCard; pReader += wcslen(pReader) + 1)
		{
			kprintf(L"| * %s\n", pReader);
			scStatus = SCardConnect(hContext, pReader, SCARD_SHARE_SHARED, SCARD_PROTOCOL_Tx, &hCard, &dwLen);
			if (scStatus == SCARD_S_SUCCESS)
			{
				dwAtrLen = SCARD_AUTOALLOCATE;
				scStatus = SCardGetAttrib(hCard, SCARD_ATTR_ATR_STRING, (PBYTE)&atr, &dwAtrLen);
				if (scStatus == SCARD_S_SUCCESS)
				{
					kprintf(L"|   ATR   : ");
					printhex(atr, dwAtrLen);
					kprintf(L"\n");
					dwLen = SCARD_AUTOALLOCATE;
					scStatus = SCardListCards(hContext, atr, NULL, 0, (LPWSTR)&mszCards, &dwLen);
					if (scStatus == SCARD_S_SUCCESS)
					{
						for (pCard = mszCards; pCard && *pCard; pCard += wcslen(pCard) + 1)
						{
							kprintf(L"|   Modele: %s\n", pCard);
						}

						if (mszCards)
						{
							SCardFreeMemory(hContext, mszCards);
						}
					}
					else PRINT_ERROR(L"SCardListCards: 0x%08x\n", scStatus);
					SCardFreeMemory(hContext, atr);
				}
				else PRINT_ERROR(L"SCardGetAttrib: 0x%08x (%u)\n", scStatus, dwAtrLen);
			}
			else if (scStatus != SCARD_W_REMOVED_CARD)
			{
				PRINT_ERROR(L"SCardConnect: 0x%08x\n", scStatus);
			}
		}

		if (mszReaders)
		{
			SCardFreeMemory(hContext, mszReaders);
		}
	}
	else if (scStatus == SCARD_E_NO_READERS_AVAILABLE)
	{
		PRINT_ERROR(L"Pas de lecteur/token\n");
	}
	else PRINT_ERROR(L"SCardListReaders: 0x%08x\n", scStatus);

	return hCard;
}

BOOLEAN SendRecvISO(SCARDHANDLE hCard, LPCBYTE pbSendBuffer, DWORD cbSendBuffer, LPBYTE pbRecvBuffer, DWORD cbRecvBuffer, PDWORD pcbReceived)
{
	BOOLEAN status = FALSE;
	LONG scStatus;
	WORD isoStatus;

	*pcbReceived = cbRecvBuffer;
	scStatus = SCardTransmit(hCard, SCARD_PCI_T0, pbSendBuffer, cbSendBuffer, NULL, pbRecvBuffer, pcbReceived);
	if (scStatus == SCARD_S_SUCCESS)
	{
		if (*pcbReceived >= 2)
		{
			isoStatus = _byteswap_ushort(*(PWORD)(pbRecvBuffer + (*pcbReceived - 2)));
			if (isoStatus == 0x9000)
			{
				status = TRUE;
				*pcbReceived -= sizeof(WORD);
			}
			else if (((isoStatus & 0x6300) == 0x6300) && (cbSendBuffer > 1) && (pbSendBuffer[1] == 0x82))
			{
				PRINT_ERROR(L"Mauvaise response envoyee a la carte (%04x)\n", isoStatus);
				if ((isoStatus & 0x00c0) == 0x00c0)
				{
					PRINT_ERROR(L"/!\\ %hhu tentative(s) restante(s) /!\\\n", isoStatus & 0x000f);
				}
			}
			else PRINT_ERROR(L"ISO Status: %04x\n", isoStatus);
		}
		else PRINT_ERROR(L"cbReceived: %u\n", *pcbReceived);
	}
	else PRINT_ERROR(L"SCardTransmit: 0x%08x\n", scStatus);

	return status;
}