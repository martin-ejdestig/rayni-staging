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

#include <cmath>
#include <iomanip>
#include <sstream>

#include "lib/time/duration_formatter.h"

namespace Rayni
{
	std::string DurationFormatter::format(unsigned int hh, unsigned int mm, float ss) const
	{
		std::ostringstream stream;

		if (hh > 0)
			stream << hh << ":";

		if (hh > 0 || mm > 0)
			stream << std::setw(2) << std::setfill('0') << mm << ":";

		bool seconds_has_dot = seconds_precision > 0;
		int seconds_width = 2 + (seconds_has_dot ? 1 : 0) + seconds_precision;

		stream << std::setw(seconds_width) << std::setfill('0') << std::fixed
		       << std::setprecision(seconds_precision) << (floor_seconds ? std::floor(ss) : ss);

		return stream.str();
	}
}