/**
 * This file is part of Rayni.
 *
 * Copyright (C) 2016 Martin Ejdestig <marejde@gmail.com>
 *
 * Rayni is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Rayni is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Rayni. If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtest/gtest.h>

#include <experimental/filesystem>
#include <fstream>

#include "lib/system/scoped_temp_dir.h"

namespace Rayni
{
	TEST(ScopedTempDirTest, RemovedWhenDestroyed)
	{
		auto output_to_file = [](auto path, auto content) {
			std::ofstream stream(path);
			stream << content;
			if (!stream.good())
				FAIL() << "Failed to write to file " << path << ".";
		};

		std::experimental::filesystem::path path;

		{
			ScopedTempDir dir;
			path = dir.get_path();

			output_to_file(path / "foo", "bla bla");

			// TODO: Bug in libstdc++ 6.0.22. remove_all() does not recursively remove.
			//       See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=71313 . Hope that
			//       it is fixed in a point release soon.
			// std::experimental::filesystem::create_directory(path / "bar");
			// output_to_file(path / "bar" / "baz", "yada yada");
		}

		EXPECT_FALSE(std::experimental::filesystem::exists(path));
	}
}