#ifndef __API_EXPORT_H__
#define __API_EXPORT_H__

#if defined WIN32 || defined _WIN32
#define DLL_CDECL __cdecl
#define DLL_STDCALL __stdcall
#else
#define DLL_CDECL
#define DLL_STDCALL
#endif

#ifndef DLL_EXTERN_C
#ifdef __cplusplus
#define DLL_EXTERN_C extern "C"
#define DLL_DEFAULT(val) = val
#else
#define DLL_EXTERN_C
#define DLL_DEFAULT(val)
#endif
#endif

#ifndef DLL_EXTERN_C_FUNCPTR
#ifdef __cplusplus
#define DLL_EXTERN_C_FUNCPTR(x) extern "C" { typedef x; }
#else
#define DLL_EXTERN_C_FUNCPTR(x) typedef x
#endif
#endif

#ifndef DLL_INLINE
#if defined __cplusplus
#define DLL_INLINE inline
#elif (defined WIN32 || defined _WIN32 || defined WINCE) && !defined __GNUC__
#define DLL_INLINE __inline
#else
#define DLL_INLINE static
#endif
#endif /* DLL_INLINE */

#if (defined WIN32 || defined _WIN32 || defined WINCE) && defined DLLAPI_EXPORTS
#define DLL_EXPORTS __declspec(dllexport)
#else
#define DLL_EXPORTS 
#endif

#ifndef DLLAPI
#define DLLAPI(rettype) DLL_EXTERN_C DLL_EXPORTS rettype DLL_CDECL
#endif

// TODO: use DLLAPI(return_type) before your functions or classes
//       For windows dll projects,add the DLLAPI_EXPORTS macro to export tyhmbols
#endif /* __API_EXPORT_H__ */