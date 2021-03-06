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

#ifndef RAYNI_LIB_MATH_MATRIX_INVERSE_H
#define RAYNI_LIB_MATH_MATRIX_INVERSE_H

#include <array>
#include <cassert>
#include <cmath>

#include "lib/math/math.h"

namespace Rayni
{
	// In-place inverse of (non-singular) matrix.
	//
	// Calling this method with a singular matrix is considered a programming error.
	//
	// Uses Gauss-Jordan elimination with partial (row) pivoting to increase numerical stability.
	template <typename Matrix>
	static constexpr inline void matrix_inverse_in_place(Matrix &m)
	{
		struct PivotPosition
		{
			unsigned int row = 0;
			unsigned int column = 0;
		};

		std::array<PivotPosition, Matrix::SIZE> pivot_positions;
		std::array<unsigned int, Matrix::SIZE> pivot_used = {};

		for (auto &pivot_position : pivot_positions) {
			real_t max = 0;

			for (unsigned int row = 0; row < Matrix::SIZE; row++) {
				if (pivot_used[row] == 1)
					continue;

				for (unsigned int column = 0; column < Matrix::SIZE; column++) {
					if (pivot_used[column] == 0 && std::abs(m(row, column)) >= max) {
						max = std::abs(m(row, column));
						pivot_position = {row, column};
					}
				}
			}

			if (pivot_position.row != pivot_position.column)
				m.swap_rows(pivot_position.row, pivot_position.column);

			unsigned int pos = pivot_position.column;
			assert(max > 0 && pivot_used[pos] == 0); // Singular matrix?
			pivot_used[pos]++;

			real_t pivot_inv = 1 / m(pos, pos);
			m(pos, pos) = 1;
			m.row(pos) *= pivot_inv;

			for (unsigned int row = 0; row < Matrix::SIZE; row++) {
				if (row != pos) {
					real_t old_value = m(row, pos);
					m(row, pos) = 0;
					m.row(row) += m.row(pos) * -old_value;
				}
			}
		}

		for (unsigned int i = Matrix::SIZE; i > 0; i--)
			if (pivot_positions[i - 1].row != pivot_positions[i - 1].column)
				m.swap_columns(pivot_positions[i - 1].row, pivot_positions[i - 1].column);
	}

	template <typename Matrix>
	static constexpr inline Matrix matrix_inverse(const Matrix &m)
	{
		Matrix ret(m);
		matrix_inverse_in_place(ret);
		return ret;
	}
}

#endif // RAYNI_LIB_MATH_MATRIX_INVERSE_H
