# p11proxy

**`p11proxy`** is a little `PKCS#11` Proxy designed to deal with PUK on Gemalto IDPrime smartcards.


## Usage

Use `p11proxy_{arch}.dll` instead of `idprimepkcs11{arch}.dll` in your program.  
Be sure that the original `idprimepkcs11{arch}.dll` can be found from the program point of view (path, ...).


## Gemalto IDPrime Admin key / PUK normal behavior

A PUK (like a PIN) is not always 4 to 8 numbers digits. It can be alphanumeric, binary, 12 bytes length... it depends on vendor/card/container policy.  
Gemalto use a PUK policy of 24 binary bytes, and default is `000000000000000000000000000000000000000000000000` (in hex).

Gemalto `PKCS#11` libraries (and `PKCS#11` standard) support this kind of PUK in binary, so if you code your own program, this should not really be a problem.  
On the other hand, it can be problematic with some tools that does not support hexadecimal input, and/or this length.


## P11 Proxy to Gemalto IDPrime behavior

By default, this proxy will intercept functions that can use PUK operations:
- `C_InitToken`
- `C_Login`
- `C_SetPIN`

If the length of the binary representation of the provided PUK code is less than 24 bytes, it will be padded (with 0).  
By ex: a provided PUK code of '12345678' (in ASCII) will be transformed in `313233343536373800000000000000000000000000000000` (in hex).

### Bonus
Because of a trick in the code, if the provided PUK code is '0000' or '00000000' (in ASCII), the default PUK code will be used: `000000000000000000000000000000000000000000000000` not `3030...0000` (in hex).  
It allows Gemalto Tools like `Minidriver Manager` to login as Admin without any modification.


## Clarification

You can use in the header file:
- `PR_FW` macro to forward a `PKCS#11` standard fonction to the original one in `idprimepkcs11{arch}.dll`
  - By eg: `PR_FW("C_CopyObject")` will forward `p11proxy_{arch}.C_CopyObject` to `idprimepkcs11{arch}.C_CopyObject`
- `PR_HK` macro to forward a `PKCS#11` standard fonction to a new one called `HOOK_C_*` ;
  - By eg: `PR_HK("C_Login")` will forward `p11proxy_{arch}.C_Login` to the internal function `HOOK_C_Login` (it's better to call the original `C_Login` somewhere inside the hook function to get the normal behavior).

Don't forget to update the hooked function list in `HOOK_C_GetFunctionList` too.  
Some programs uses pointers to function from `C_GetFunctionList`, some from the DLL exported functions, so they must be hooked on both sides.


## Remarks

- This project is not affiliated with Gemalto and official `PKCS#11` libraries are not provided.  
  See: http://support.gemalto.com/index.php?id=where_can_i_download_the_pkcs11.
- I had problem with `Pkcs11Admin` tool, even with the Gemalto original DLL...


## Author
Benjamin DELPY `gentilkiwi`, you can contact me on Twitter ( @gentilkiwi ) or by mail ( benjamin [at] gentilkiwi.com )
