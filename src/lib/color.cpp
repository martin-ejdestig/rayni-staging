/**
 * This file is part of Rayni.
 *
 * Copyright (C) 2015 Martin Ejdestig <marejde@gmail.com>
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

#include "lib/color.h"
#include "lib/containers/variant.h"

namespace Rayni
{
	Color Color::from_variant(const Variant &v)
	{
		if (v.is_string())
		{
			const std::string &str = v.get_string();

			if (str == "black")
				return Color::black();
			if (str == "white")
				return Color::white();
			if (str == "red")
				return Color::red();
			if (str == "yellow")
				return Color::yellow();
			if (str == "green")
				return Color::green();
			if (str == "blue")
				return Color::blue();

			throw Variant::Exception(v, "unknown color \"" + str + "\"");
		}
		else if (v.is_vector())
		{
			real_t r = v.get<real_t>(0);
			real_t g = v.get<real_t>(1);
			real_t b = v.get<real_t>(2);
			return Color(r, g, b).clamp();
		}

		throw Variant::Exception(v, "color must be a string or vector");
	}
}