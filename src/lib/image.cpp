/**
 * This file is part of Rayni.
 *
 * Copyright (C) 2013-2015 Martin Ejdestig <marejde@gmail.com>
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

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>

#include "lib/color.h"
#include "lib/image.h"

namespace Rayni
{
	Image::Image()
	{
	}

	Image::Image(Image &&other) noexcept : width(other.width), height(other.height), buffer(std::move(other.buffer))
	{
		other.width = 0;
		other.height = 0;
	}

	Image::Image(unsigned int width, unsigned int height)
	        : width(width), height(height), buffer(get_stride() * height)
	{
		for (unsigned int i = 0; i < buffer.size(); i += BYTES_PER_PIXEL)
		{
			buffer[i + A_PIXEL_OFFSET] = 0xff;
			buffer[i + R_PIXEL_OFFSET] = 0x00;
			buffer[i + G_PIXEL_OFFSET] = 0x00;
			buffer[i + B_PIXEL_OFFSET] = 0x00;
		}
	}

	Image &Image::operator=(Image &&other) noexcept
	{
		assert(this != &other);

		width = other.width;
		other.width = 0;
		height = other.height;
		other.height = 0;
		buffer = std::move(other.buffer);

		return *this;
	}

	void Image::write_pixel(unsigned int x, unsigned int y, const Color &color)
	{
		std::size_t i = offset_to(x, y);

		buffer[i + R_PIXEL_OFFSET] = color.r() < 1 ? static_cast<std::uint8_t>(color.r() * 256) : 255;
		buffer[i + G_PIXEL_OFFSET] = color.g() < 1 ? static_cast<std::uint8_t>(color.g() * 256) : 255;
		buffer[i + B_PIXEL_OFFSET] = color.b() < 1 ? static_cast<std::uint8_t>(color.b() * 256) : 255;
	}

	Color Image::read_pixel(unsigned int x, unsigned int y) const
	{
		Color color;
		std::size_t i = offset_to(x, y);

		color.r() = static_cast<real_t>(buffer[i + R_PIXEL_OFFSET]) / 255;
		color.g() = static_cast<real_t>(buffer[i + G_PIXEL_OFFSET]) / 255;
		color.b() = static_cast<real_t>(buffer[i + B_PIXEL_OFFSET]) / 255;

		return color;
	}

	inline std::size_t Image::offset_to(unsigned int x, unsigned int y) const
	{
		return get_stride() * y + x * BYTES_PER_PIXEL;
	}
}