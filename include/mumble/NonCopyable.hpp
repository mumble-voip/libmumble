// This file is part of libmumble.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_NONCOPYABLE_HPP
#define MUMBLE_NONCOPYABLE_HPP

namespace mumble {
class NonCopyable {
public:
	NonCopyable()  = default;
	~NonCopyable() = default;

private:
	NonCopyable(const NonCopyable &)            = delete;
	NonCopyable &operator=(const NonCopyable &) = delete;
};
} // namespace mumble

#endif
