#pragma once
#include <type_traits>
#include <Windows.h>

template <class T, class... Types> constexpr bool IsAnyOf_v = std::disjunction_v<std::is_same<T, Types>...>;

//the nice thing is you can change type requirement based on if unicode is enabled
template<class T> constexpr bool IsWindowsTypes_v = IsAnyOf_v<std::remove_cv_t<T>,
    WORD, DWORD, 
#ifdef UNICODE //types to accept when unicode is enabled
    WCHAR, LPWSTR, LPWCH >;
#else //types to accept when unicode is not enabled
    CHAR, LPSTR, LPCH  > ;
#endif

template<class T> void  CheckIsWindowsType() { static_assert(IsWindowsTypes_v<T>, "Type is not Windows type"); }
template<class... Types> void CheckIsWindowsTypes() { (CheckIsWindowsType<Types>(), ...); }