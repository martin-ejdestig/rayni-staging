// This file is part of Rayni.
//
// Copyright (C) 2013-2021 Martin Ejdestig <marejde@gmail.com>
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

#include "lib/system/unique_fd.h"

#include <gtest/gtest.h>
#include <sys/eventfd.h>

#include <utility>

namespace
{
	int test_fd()
	{
		// #if:ery here for other OS:es when adding support... and remove this comment.
		int fd = eventfd(0, EFD_CLOEXEC);

		return fd;
	}
}

namespace Rayni
{
	TEST(UniqueFD, MinusOneByDefault)
	{
		UniqueFD fd;

		EXPECT_EQ(-1, fd.get());
	}

	TEST(UniqueFD, MoveConstructor)
	{
		UniqueFD fd1(test_fd());
		int fd = fd1.get();
		ASSERT_NE(-1, fd);

		UniqueFD fd2(std::move(fd1));

		// NOLINTNEXTLINE(bugprone-use-after-move, clang-analyzer-cplusplus.Move) Tests move.
		EXPECT_EQ(-1, fd1.get());
		EXPECT_EQ(fd, fd2.get());
	}

	TEST(UniqueFD, MoveAssignment)
	{
		UniqueFD fd1(test_fd());
		int fd = fd1.get();
		ASSERT_NE(-1, fd);

		UniqueFD fd2;
		fd2 = std::move(fd1);

		// NOLINTNEXTLINE(bugprone-use-after-move, clang-analyzer-cplusplus.Move) Tests move.
		EXPECT_EQ(-1, fd1.get());
		EXPECT_EQ(fd, fd2.get());
	}

	TEST(UniqueFD, Close)
	{
		UniqueFD fd(test_fd());

		ASSERT_NE(-1, fd.get());
		fd.close();
		EXPECT_EQ(-1, fd.get());
	}

	TEST(UniqueFD, CloseNotSet)
	{
		UniqueFD fd;

		EXPECT_EQ(-1, fd.get());
		fd.close();
		EXPECT_EQ(-1, fd.get());
	}
}
