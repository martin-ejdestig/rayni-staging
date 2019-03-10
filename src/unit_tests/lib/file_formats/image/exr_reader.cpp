/**
 * This file is part of Rayni.
 *
 * Copyright (C) 2016-2019 Martin Ejdestig <marejde@gmail.com>
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

#include "lib/file_formats/image/exr_reader.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <string>
#include <vector>

#include "lib/graphics/image.h"
#include "lib/io/file.h"
#include "lib/system/scoped_temp_dir.h"

namespace Rayni
{
	namespace
	{
		std::vector<std::uint8_t> exr_data()
		{
			return {0x76, 0x2f, 0x31, 0x01, 0x02, 0x00, 0x00, 0x00, 0x63, 0x68, 0x61, 0x6e, 0x6e, 0x65,
			        0x6c, 0x73, 0x00, 0x63, 0x68, 0x6c, 0x69, 0x73, 0x74, 0x00, 0x37, 0x00, 0x00, 0x00,
			        0x42, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
			        0x01, 0x00, 0x00, 0x00, 0x47, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			        0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x52, 0x00, 0x01, 0x00, 0x00, 0x00,
			        0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x63,
			        0x6f, 0x6d, 0x70, 0x72, 0x65, 0x73, 0x73, 0x69, 0x6f, 0x6e, 0x00, 0x63, 0x6f, 0x6d,
			        0x70, 0x72, 0x65, 0x73, 0x73, 0x69, 0x6f, 0x6e, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
			        0x64, 0x61, 0x74, 0x61, 0x57, 0x69, 0x6e, 0x64, 0x6f, 0x77, 0x00, 0x62, 0x6f, 0x78,
			        0x32, 0x69, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			        0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x64, 0x69, 0x73, 0x70, 0x6c,
			        0x61, 0x79, 0x57, 0x69, 0x6e, 0x64, 0x6f, 0x77, 0x00, 0x62, 0x6f, 0x78, 0x32, 0x69,
			        0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
			        0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x6c, 0x69, 0x6e, 0x65, 0x4f, 0x72, 0x64,
			        0x65, 0x72, 0x00, 0x6c, 0x69, 0x6e, 0x65, 0x4f, 0x72, 0x64, 0x65, 0x72, 0x00, 0x01,
			        0x00, 0x00, 0x00, 0x00, 0x70, 0x69, 0x78, 0x65, 0x6c, 0x41, 0x73, 0x70, 0x65, 0x63,
			        0x74, 0x52, 0x61, 0x74, 0x69, 0x6f, 0x00, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x00, 0x04,
			        0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x73, 0x63, 0x72, 0x65, 0x65, 0x6e, 0x57,
			        0x69, 0x6e, 0x64, 0x6f, 0x77, 0x43, 0x65, 0x6e, 0x74, 0x65, 0x72, 0x00, 0x76, 0x32,
			        0x66, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			        0x73, 0x63, 0x72, 0x65, 0x65, 0x6e, 0x57, 0x69, 0x6e, 0x64, 0x6f, 0x77, 0x57, 0x69,
			        0x64, 0x74, 0x68, 0x00, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x00, 0x04, 0x00, 0x00, 0x00,
			        0x00, 0x00, 0x80, 0x3f, 0x00, 0x49, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5d,
			        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00,
			        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x3c, 0x00, 0x3c, 0x01,
			        0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x3c, 0x00,
			        0x00, 0x00, 0x00, 0x00, 0x00};
		}

		std::vector<std::uint8_t> corrupt_exr_data()
		{
			auto data = exr_data();
			data.at(data.size() - 20) ^= 0x01;
			return data;
		}

		std::vector<std::uint8_t> short_exr_data()
		{
			auto data = exr_data();
			data.pop_back();
			return data;
		}
	}

	TEST(EXRReader, ReadFile)
	{
		static constexpr unsigned int VALID_WIDTH = 2;
		static constexpr unsigned int VALID_HEIGHT = 2;
		static constexpr Color VALID_COLORS[VALID_HEIGHT][VALID_WIDTH] = {{Color::red(), Color::yellow()},
		                                                                  {Color::green(), Color::blue()}};
		ScopedTempDir temp_dir;

		const std::string valid_path = temp_dir.path() / "valid.exr";
		file_write(valid_path, exr_data());
		Image image = EXRReader().read_file(valid_path);

		ASSERT_EQ(VALID_WIDTH, image.width());
		ASSERT_EQ(VALID_HEIGHT, image.height());

		for (unsigned int y = 0; y < VALID_HEIGHT; y++)
		{
			for (unsigned int x = 0; x < VALID_WIDTH; x++)
			{
				Color valid_color = VALID_COLORS[y][x];
				Color color = image.read_pixel(x, y);

				EXPECT_NEAR(valid_color.r(), color.r(), 1e-100);
				EXPECT_NEAR(valid_color.g(), color.g(), 1e-100);
				EXPECT_NEAR(valid_color.b(), color.b(), 1e-100);
			}
		}

		const std::string corrupt_path = temp_dir.path() / "corrupt.exr";
		file_write(corrupt_path, corrupt_exr_data());
		EXPECT_THROW(EXRReader().read_file(corrupt_path), EXRReader::Exception);

		const std::string short_path = temp_dir.path() / "short.exr";
		file_write(short_path, short_exr_data());
		EXPECT_THROW(EXRReader().read_file(short_path), EXRReader::Exception);

		EXPECT_THROW(EXRReader().read_file(temp_dir.path() / "does_not_exist.exr"), EXRReader::Exception);
	}
}
