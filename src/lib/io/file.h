// This file is part of Rayni.
//
// Copyright (C) 2016-2021 Martin Ejdestig <marejde@gmail.com>
//
// Rayni is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Rayni is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Rayni. If not, see <http://www.gnu.org/licenses/>.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef RAYNI_LIB_IO_FILE_H
#define RAYNI_LIB_IO_FILE_H

#include <cstdint>
#include <string>
#include <vector>

#include "lib/function/result.h"

namespace Rayni
{
	Result<std::vector<std::uint8_t>> file_read(const std::string &path);

	Result<void> file_write(const std::string &path, const std::vector<std::uint8_t> &data);
}

#endif // RAYNI_LIB_IO_FILE_H
