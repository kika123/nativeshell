
/* wide function prototypes, also declared in wchar.h  */

_CRTIMP __checkReturn int __cdecl _waccess(__in_z const wchar_t * _Filename, __in int _AccessMode);
_CRTIMP __checkReturn_wat errno_t __cdecl _waccess_s(__in_z const wchar_t * _Filename, __in int _AccessMode);
_CRTIMP __checkReturn int __cdecl _wchmod(__in_z const wchar_t * _Filename, __in int _Mode);
_CRT_INSECURE_DEPRECATE(_wsopen_s) _CRTIMP __checkReturn int __cdecl _wcreat(__in_z const wchar_t * _Filename, __in int _PermissionMode);
_CRTIMP __checkReturn intptr_t __cdecl _wfindfirst32(__in_z const wchar_t * _Filename, __out struct _wfinddata32_t * _FindData);
_CRTIMP __checkReturn int __cdecl _wfindnext32(__in intptr_t _FindHandle, __out struct _wfinddata32_t * _FindData);
_CRTIMP __checkReturn int __cdecl _wunlink(__in_z const wchar_t * _Filename);
_CRTIMP __checkReturn int __cdecl _wrename(__in_z const wchar_t * _NewFilename, __in_z const wchar_t * _OldFilename);
_CRTIMP errno_t __cdecl _wmktemp_s(__inout_ecount_z(_SizeInWords) wchar_t * _TemplateName, __in size_t _SizeInWords);
__DEFINE_CPP_OVERLOAD_SECURE_FUNC_0_0(errno_t, _wmktemp_s, __inout_ecount_z(_Size) wchar_t, _TemplateName)
__DEFINE_CPP_OVERLOAD_STANDARD_FUNC_0_0(wchar_t *, __RETURN_POLICY_DST, _CRTIMP, _wmktemp, __inout_z wchar_t, _TemplateName)

_CRTIMP intptr_t __cdecl _wfindfirst(const wchar_t *, struct _wfinddata_t *);
_CRTIMP int __cdecl _wfindnext(intptr_t, struct _wfinddata_t *);

#if     _INTEGRAL_MAX_BITS >= 64
_CRTIMP __checkReturn intptr_t __cdecl _wfindfirst32i64(__in_z const wchar_t * _Filename, __out struct _wfinddata32i64_t * _FindData);
_CRTIMP __checkReturn intptr_t __cdecl _wfindfirst64i32(__in_z const wchar_t * _Filename, __out struct _wfinddata64i32_t * _FindData);
_CRTIMP __checkReturn intptr_t __cdecl _wfindfirst64(__in_z const wchar_t * _Filename, __out struct _wfinddata64_t * _FindData);
_CRTIMP __checkReturn int __cdecl _wfindnext32i64(__in intptr_t _FindHandle, __out struct _wfinddata32i64_t * _FindData);
_CRTIMP __checkReturn int __cdecl _wfindnext64i32(__in intptr_t _FindHandle, __out struct _wfinddata64i32_t * _FindData);
_CRTIMP __checkReturn int __cdecl _wfindnext64(__in intptr_t _FindHandle, __out struct _wfinddata64_t * _FindData);

_CRTIMP intptr_t __cdecl _wfindfirsti64(const wchar_t *, struct _wfinddatai64_t *);
_CRTIMP int __cdecl _wfindnexti64(intptr_t, struct _wfinddatai64_t *);

#endif

_CRTIMP __checkReturn_wat errno_t __cdecl _wsopen_s(__out int * _FileHandle, __in_z const wchar_t * _Filename, __in int _OpenFlag, __in int _ShareFlag, __in int _PermissionFlag);

_CRTIMP __checkReturn int __cdecl _wopen(__in_z const wchar_t * _Filename, __in int _OpenFlag, ...);
_CRTIMP int __cdecl _wsopen(__in_z const wchar_t * _Filename, __in int _OpenFlag, __in int _ShareFlag, ...);