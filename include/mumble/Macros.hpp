// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_MACROS_HPP
#define MUMBLE_MACROS_HPP

#define MUMBLE_ENUM_OPERATORS(T)                                                                                       \
	static T operator~(const T lhs) { return static_cast< T >(~static_cast< std::underlying_type< T >::type >(lhs)); } \
	static T operator|(const T lhs, const T rhs) {                                                                     \
		return static_cast< T >(static_cast< std::underlying_type< T >::type >(lhs)                                    \
								| static_cast< std::underlying_type< T >::type >(rhs));                                \
	}                                                                                                                  \
	static T operator&(const T lhs, const T rhs) {                                                                     \
		return static_cast< T >(static_cast< std::underlying_type< T >::type >(lhs)                                    \
								& static_cast< std::underlying_type< T >::type >(rhs));                                \
	}                                                                                                                  \
	static T operator^(const T lhs, const T rhs) {                                                                     \
		return static_cast< T >(static_cast< std::underlying_type< T >::type >(lhs)                                    \
								^ static_cast< std::underlying_type< T >::type >(rhs));                                \
	}                                                                                                                  \
	static T &operator|=(T &lhs, const T rhs) {                                                                        \
		return reinterpret_cast< T & >(reinterpret_cast< std::underlying_type< T >::type & >(lhs) |=                   \
									   static_cast< std::underlying_type< T >::type >(rhs));                           \
	}                                                                                                                  \
	static T &operator&=(T &lhs, const T rhs) {                                                                        \
		return reinterpret_cast< T & >(reinterpret_cast< std::underlying_type< T >::type & >(lhs) &=                   \
									   static_cast< std::underlying_type< T >::type >(rhs));                           \
	}                                                                                                                  \
	static T &operator^=(T &lhs, const T rhs) {                                                                        \
		return reinterpret_cast< T & >(reinterpret_cast< std::underlying_type< T >::type & >(lhs) ^=                   \
									   static_cast< std::underlying_type< T >::type >(rhs));                           \
	}

#endif
