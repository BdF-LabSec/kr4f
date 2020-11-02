/*	Benjamin DELPY `gentilkiwi`
	LabSec - DGSI DIT ARCOS
	benjamin.delpy@banque-france.fr / benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "p11proxy.h"

static CK_FUNCTION_LIST funcList;
static CK_FUNCTION_LIST_PTR funcListPtr = &funcList;
CK_RV CK_CALL_SPEC HOOK_C_GetFunctionList(CK_FUNCTION_LIST_PTR_PTR ppFunctionList)
{
	CK_RV rv;
	CK_FUNCTION_LIST_PTR ptr;
	rv = C_GetFunctionList(&ptr);
	if(rv == CKR_OK)
	{
		funcList = *ptr;
		funcList.C_GetFunctionList = HOOK_C_GetFunctionList;
		funcList.C_InitToken = HOOK_C_InitToken;
		funcList.C_Login = HOOK_C_Login;
		funcList.C_SetPIN = HOOK_C_SetPIN;
		*ppFunctionList = funcListPtr;
	}
	return rv;
}

CK_RV CK_CALL_SPEC HOOK_C_InitToken(CK_SLOT_ID slotID, CK_UTF8CHAR_PTR pPin, CK_ULONG ulPinLen, CK_UTF8CHAR_PTR pLabel)
{
	CK_RV rv;
	CK_BYTE_PTR puk;
	CK_ULONG pukLen;
	rv = AdaptPukToValidPuk(pPin, ulPinLen, &puk, &pukLen);
	if(rv == CKR_OK)
	{
		rv = C_InitToken(slotID, puk, pukLen, pLabel);
		FreeValidPuk(puk);
	}
	return rv;
}

CK_RV CK_CALL_SPEC HOOK_C_Login(CK_SESSION_HANDLE hSession, CK_USER_TYPE userType, CK_UTF8CHAR_PTR pPin, CK_ULONG ulPinLen)
{
	CK_RV rv;
	CK_BYTE_PTR puk;
	CK_ULONG pukLen;
	if (userType != CKU_SO)
	{
		rv = C_Login(hSession, userType, pPin, ulPinLen);
	}
	else
	{
		rv = AdaptPukToValidPuk(pPin, ulPinLen, &puk, &pukLen);
		if(rv == CKR_OK)
		{
			rv = C_Login(hSession, userType, puk, pukLen);
			FreeValidPuk(puk);
		}
	}
	return rv;
}

CK_RV CK_CALL_SPEC HOOK_C_SetPIN(CK_SESSION_HANDLE hSession, CK_UTF8CHAR_PTR pOldPin, CK_ULONG ulOldLen, CK_UTF8CHAR_PTR pNewPin, CK_ULONG ulNewLen)
{
	CK_RV rv;
	CK_SESSION_INFO info;
	CK_BYTE_PTR oldPuk;
	CK_ULONG oldPukLen;
	rv = C_GetSessionInfo(hSession, &info);
	if(rv == CKR_OK)
	{
		if (info.state != CKS_RW_SO_FUNCTIONS)
		{
			rv = C_SetPIN(hSession, pOldPin, ulOldLen, pNewPin, ulNewLen);
		}
		else
		{
			rv = AdaptPukToValidPuk(pOldPin, ulOldLen, &oldPuk, &oldPukLen);
			if(rv == CKR_OK)
			{
				rv = C_SetPIN(hSession, oldPuk, oldPukLen, pNewPin, ulNewLen);
				FreeValidPuk(oldPuk);
			}
		}
	}
	return rv;
}

CK_RV AdaptPukToValidPuk(CK_UTF8CHAR_PTR pPuk, CK_ULONG ulPukLen, CK_UTF8CHAR_PTR *ppValidPuk, CK_ULONG *pulValidPukLen)
{
	CK_RV status;
	if(pPuk && ulPukLen && ppValidPuk && pulValidPukLen)
	{
		*pulValidPukLen = 0;
		*ppValidPuk = (CK_UTF8CHAR_PTR)LocalAlloc(LPTR, max(24, ulPukLen)); // 0x00 inside
		if(*ppValidPuk)
		{
			if ((ulPukLen == sizeof(ULONGLONG)) && (*((PULONGLONG)pPuk) == 0x3030303030303030))
			{
				*pulValidPukLen = 24;
			}
			else
			{
				*pulValidPukLen = ulPukLen;
				RtlCopyMemory(*ppValidPuk, pPuk, *pulValidPukLen);
			}
			status = CKR_OK;
		}
		else status = CKR_HOST_MEMORY;
	}
	else status = CKR_ARGUMENTS_BAD;
	return status;
}

void FreeValidPuk(CK_UTF8CHAR_PTR pValidPuk)
{
	if (pValidPuk)
	{
		LocalFree(pValidPuk);
	}
}