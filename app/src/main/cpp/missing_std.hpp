// Copyright (c) 2016 Roman Beránek. All rights reserved.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#if __cplusplus < 201600L

namespace std {
    template<class C>
    constexpr auto size(const C &c) -> decltype(c.size()) {
        return c.size();
    }

    template<class C>
    constexpr auto empty(const C &c) -> decltype(c.empty()) {
        return c.empty();
    }
}

#endif
