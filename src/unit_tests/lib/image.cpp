/**
 * This file is part of Rayni.
 *
 * Copyright (C) 2013-2016 Martin Ejdestig <marejde@gmail.com>
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

#include <cmath>
#include <vector>

#include "lib/color.h"
#include "lib/image.h"

namespace Rayni
{
	class ImageTest : public testing::Test
	{
	protected:
		struct Pixel
		{
			unsigned int x, y;
			Color color;
		};

		static testing::AssertionResult expect_color_at(const char *image_expr,
		                                                const char * /*x_expr*/,
		                                                const char * /*y_expr*/,
		                                                const char *color_expr,
		                                                const Image &image,
		                                                unsigned int x,
		                                                unsigned int y,
		                                                const Color &color)
		{
			static const real_t COMPONENT_MAX_DIFF = 0.001;

			if (x > image.get_width() || y > image.get_height())
				return testing::AssertionFailure() << image_expr << " does not contain (" << x << ", "
				                                   << y << ") (too small)";

			Color image_color = image.read_pixel(x, y);

			if (std::abs(color.r() - image_color.r()) > COMPONENT_MAX_DIFF ||
			    std::abs(color.g() - image_color.g()) > COMPONENT_MAX_DIFF ||
			    std::abs(color.b() - image_color.b()) > COMPONENT_MAX_DIFF)
			{
				return testing::AssertionFailure()
				       << color_expr << " and " << image_expr << " color at (" << x << ", " << y
				       << ") componentwise difference is (" << color.r() - image_color.r() << ", "
				       << color.g() - image_color.g() << ", " << color.b() - image_color.b() << ").";
			}

			return testing::AssertionSuccess();
		}

		static const std::vector<Pixel> &pixels_2x2()
		{
			static const std::vector<Pixel> pixels = {{0, 0, Color::black()},
			                                          {0, 1, Color::red()},
			                                          {1, 0, Color::green()},
			                                          {1, 1, Color::blue()}};
			return pixels;
		}

		static void write_pixels(Image &image, const std::vector<Pixel> &pixels)
		{
			for (auto &pixel : pixels)
				image.write_pixel(pixel.x, pixel.y, pixel.color);
		}

		static testing::AssertionResult expect_pixels(const char *image_expr,
		                                              const char *pixels_expr,
		                                              const Image &image,
		                                              const std::vector<Pixel> &pixels)
		{
			for (std::vector<Pixel>::size_type i = 0; i < pixels.size(); i++)
			{
				std::string pixels_expr_i = std::string(pixels_expr) + "[" + std::to_string(i) + "]";
				auto result = expect_color_at(image_expr,
				                              (pixels_expr_i + ".x").c_str(),
				                              (pixels_expr_i + ".y").c_str(),
				                              (pixels_expr_i + ".color").c_str(),
				                              image,
				                              pixels[i].x,
				                              pixels[i].y,
				                              pixels[i].color);
				if (!result)
					return result;
			}

			return testing::AssertionSuccess();
		}
	};

	TEST_F(ImageTest, Size)
	{
		const unsigned int WIDTH = 4;
		const unsigned int HEIGHT = 2;
		Image image(WIDTH, HEIGHT);
		Image empty_image;

		EXPECT_FALSE(image.is_empty());
		EXPECT_EQ(WIDTH, image.get_width());
		EXPECT_EQ(HEIGHT, image.get_height());
		EXPECT_LE(WIDTH, image.get_stride());

		EXPECT_TRUE(empty_image.is_empty());
		EXPECT_EQ(0, empty_image.get_width());
		EXPECT_EQ(0, empty_image.get_height());
		EXPECT_EQ(0, empty_image.get_stride());
	}

	TEST_F(ImageTest, Area)
	{
		const unsigned int WIDTH = 4;
		const unsigned int HEIGHT = 2;
		Image::Area area = Image(WIDTH, HEIGHT).whole_area();

		EXPECT_EQ(0, area.x);
		EXPECT_EQ(0, area.y);
		EXPECT_EQ(WIDTH, area.width);
		EXPECT_EQ(HEIGHT, area.height);
	}

	TEST_F(ImageTest, BlackByDefault)
	{
		Image image(2, 2);

		for (unsigned int y = 0; y < image.get_height(); y++)
			for (unsigned int x = 0; x < image.get_width(); x++)
				EXPECT_PRED_FORMAT4(expect_color_at, image, x, y, Color::black());
	}

	TEST_F(ImageTest, Pixels)
	{
		Image image(2, 2);
		write_pixels(image, pixels_2x2());
		EXPECT_PRED_FORMAT2(expect_pixels, image, pixels_2x2());
	}

	TEST_F(ImageTest, MoveConstructor)
	{
		Image image1(2, 2);
		write_pixels(image1, pixels_2x2());
		Image image2(std::move(image1));

		EXPECT_PRED_FORMAT2(expect_pixels, image2, pixels_2x2());
		EXPECT_TRUE(image1.is_empty());
	}

	TEST_F(ImageTest, MoveAssignment)
	{
		Image image1(2, 2);
		write_pixels(image1, pixels_2x2());
		Image image2 = std::move(image1);

		EXPECT_PRED_FORMAT2(expect_pixels, image2, pixels_2x2());
		EXPECT_TRUE(image1.is_empty());
	}
}
