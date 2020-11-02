/*	Benjamin DELPY `gentilkiwi`
	LabSec - DGSI DIT ARCOS
	benjamin.delpy@banque-france.fr / benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#pragma once
#include <stdio.h>
#include <windows.h>
#include "pkcs11/cryptoki.h"

#ifdef _M_X64
#define ORIGLIB	"idprimepkcs1164"
#define CALL_SPEC_PRE
#else
#define ORIGLIB	"idprimepkcs11"
#define CALL_SPEC_PRE "_"
#endif

__pragma(comment(lib, "lib/" ORIGLIB ".lib"))

#define PROXY_FORWARD_ORIGINAL(name)	__pragma(comment(linker, "/export:" #name "=" ORIGLIB "." #name))
#define PROXY_HOOK(name)	__pragma(comment(linker, "/export:" #name "=" CALL_SPEC_PRE "HOOK_" #name))

CK_RV CK_CALL_SPEC HOOK_C_GetFunctionList(CK_FUNCTION_LIST_PTR_PTR ppFunctionList);
CK_RV CK_CALL_SPEC HOOK_C_InitToken(CK_SLOT_ID slotID, CK_UTF8CHAR_PTR pPin, CK_ULONG ulPinLen, CK_UTF8CHAR_PTR pLabel);
CK_RV CK_CALL_SPEC HOOK_C_Login(CK_SESSION_HANDLE hSession, CK_USER_TYPE userType, CK_UTF8CHAR_PTR pPin, CK_ULONG ulPinLen);
CK_RV CK_CALL_SPEC HOOK_C_SetPIN(CK_SESSION_HANDLE hSession, CK_UTF8CHAR_PTR pOldPin, CK_ULONG ulOldLen, CK_UTF8CHAR_PTR pNewPin, CK_ULONG ulNewLen);

__declspec(noinline) CK_RV AdaptPukToValidPuk(CK_UTF8CHAR_PTR pPuk, CK_ULONG ulPukLen, CK_UTF8CHAR_PTR *ppValidPuk, CK_ULONG *pulValidPukLen);
__declspec(noinline) void FreeValidPuk(CK_UTF8CHAR_PTR pValidPuk);

PROXY_FORWARD_ORIGINAL("C_CancelFunction")
PROXY_FORWARD_ORIGINAL("C_CloseAllSessions")
PROXY_FORWARD_ORIGINAL("C_CloseSession")
PROXY_FORWARD_ORIGINAL("C_CopyObject")
PROXY_FORWARD_ORIGINAL("C_CreateObject")
PROXY_FORWARD_ORIGINAL("C_Decrypt")
PROXY_FORWARD_ORIGINAL("C_DecryptDigestUpdate")
PROXY_FORWARD_ORIGINAL("C_DecryptFinal")
PROXY_FORWARD_ORIGINAL("C_DecryptInit")
PROXY_FORWARD_ORIGINAL("C_DecryptUpdate")
PROXY_FORWARD_ORIGINAL("C_DecryptVerifyUpdate")
PROXY_FORWARD_ORIGINAL("C_DeriveKey")
PROXY_FORWARD_ORIGINAL("C_DestroyObject")
PROXY_FORWARD_ORIGINAL("C_Digest")
PROXY_FORWARD_ORIGINAL("C_DigestEncryptUpdate")
PROXY_FORWARD_ORIGINAL("C_DigestFinal")
PROXY_FORWARD_ORIGINAL("C_DigestInit")
PROXY_FORWARD_ORIGINAL("C_DigestKey")
PROXY_FORWARD_ORIGINAL("C_DigestUpdate")
PROXY_FORWARD_ORIGINAL("C_Encrypt")
PROXY_FORWARD_ORIGINAL("C_EncryptFinal")
PROXY_FORWARD_ORIGINAL("C_EncryptInit")
PROXY_FORWARD_ORIGINAL("C_EncryptUpdate")
PROXY_FORWARD_ORIGINAL("C_Finalize")
PROXY_FORWARD_ORIGINAL("C_FindObjects")
PROXY_FORWARD_ORIGINAL("C_FindObjectsFinal")
PROXY_FORWARD_ORIGINAL("C_FindObjectsInit")
PROXY_FORWARD_ORIGINAL("C_GenerateKey")
PROXY_FORWARD_ORIGINAL("C_GenerateKeyPair")
PROXY_FORWARD_ORIGINAL("C_GenerateRandom")
PROXY_FORWARD_ORIGINAL("C_GetAttributeValue")
PROXY_FORWARD_ORIGINAL("C_GetCardProperty")
PROXY_HOOK("C_GetFunctionList") //
PROXY_FORWARD_ORIGINAL("C_GetFunctionStatus")
PROXY_FORWARD_ORIGINAL("C_GetInfo")
PROXY_FORWARD_ORIGINAL("C_GetMechanismInfo")
PROXY_FORWARD_ORIGINAL("C_GetMechanismList")
PROXY_FORWARD_ORIGINAL("C_GetObjectSize")
PROXY_FORWARD_ORIGINAL("C_GetOperationState")
PROXY_FORWARD_ORIGINAL("C_GetSessionInfo")
PROXY_FORWARD_ORIGINAL("C_GetSlotInfo")
PROXY_FORWARD_ORIGINAL("C_GetSlotList")
PROXY_FORWARD_ORIGINAL("C_GetTokenInfo")
PROXY_FORWARD_ORIGINAL("C_InitPIN")
PROXY_HOOK("C_InitToken") //
PROXY_FORWARD_ORIGINAL("C_Initialize")
PROXY_HOOK("C_Login") //
PROXY_FORWARD_ORIGINAL("C_Logout")
PROXY_FORWARD_ORIGINAL("C_OpenSession")
PROXY_FORWARD_ORIGINAL("C_SeedRandom")
PROXY_FORWARD_ORIGINAL("C_SetAttributeValue")
PROXY_FORWARD_ORIGINAL("C_SetCardProperty")
PROXY_FORWARD_ORIGINAL("C_SetOperationState")
PROXY_HOOK("C_SetPIN"); //
PROXY_FORWARD_ORIGINAL("C_Sign")
PROXY_FORWARD_ORIGINAL("C_SignEncryptUpdate")
PROXY_FORWARD_ORIGINAL("C_SignFinal")
PROXY_FORWARD_ORIGINAL("C_SignInit")
PROXY_FORWARD_ORIGINAL("C_SignRecover")
PROXY_FORWARD_ORIGINAL("C_SignRecoverInit")
PROXY_FORWARD_ORIGINAL("C_SignUpdate")
PROXY_FORWARD_ORIGINAL("C_UnwrapKey")
PROXY_FORWARD_ORIGINAL("C_Verify")
PROXY_FORWARD_ORIGINAL("C_VerifyFinal")
PROXY_FORWARD_ORIGINAL("C_VerifyInit")
PROXY_FORWARD_ORIGINAL("C_VerifyRecover")
PROXY_FORWARD_ORIGINAL("C_VerifyRecoverInit")
PROXY_FORWARD_ORIGINAL("C_VerifyUpdate")
PROXY_FORWARD_ORIGINAL("C_WaitForSlotEvent")
PROXY_FORWARD_ORIGINAL("C_WrapKey")