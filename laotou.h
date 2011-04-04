#if !defined(LAOTOU_H__E20B6F4A_871F_4E26_A9A7_E9FC64322437__INCLUDED_)
#define LAOTOU_H__E20B6F4A_871F_4E26_A9A7_E9FC64322437__INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif
DWORD WINAPI
LaotouMountImage(
    LPWSTR lpszMountPath,
    LPWSTR lpszWimFileName,
    DWORD  dwImageIndex,
    LPWSTR lpszTempPath
    );
#ifdef __cplusplus
}
#endif
#endif // !defined(LAOTOU_H__E20B6F4A_871F_4E26_A9A7_E9FC64322437__INCLUDED_)
