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

#include <utility>

#include "lib/containers/variant.h"
#include "lib/math/quaternion.h"

namespace Rayni
{
	TEST(QuaternionTest, Variant)
	{
		auto vector_variant = [](unsigned int size)
		{
			Variant::Vector v;
			for (unsigned int i = 0; i < size; i++)
				v.emplace_back(i + 1);
			return Variant(std::move(v));
		};

		Quaternion q = vector_variant(4).to<Quaternion>();
		EXPECT_NEAR(1, q.x(), 1e-100);
		EXPECT_NEAR(2, q.y(), 1e-100);
		EXPECT_NEAR(3, q.z(), 1e-100);
		EXPECT_NEAR(4, q.w(), 1e-100);

		EXPECT_THROW(vector_variant(0).to<Quaternion>(), Variant::Exception);
		EXPECT_THROW(vector_variant(1).to<Quaternion>(), Variant::Exception);
		EXPECT_THROW(vector_variant(2).to<Quaternion>(), Variant::Exception);
		EXPECT_THROW(vector_variant(3).to<Quaternion>(), Variant::Exception);
		EXPECT_NO_THROW(vector_variant(5).to<Quaternion>()); // TODO: Reconsider?

		EXPECT_THROW(Variant(0).to<Quaternion>(), Variant::Exception);
	}

	TEST(QuaternionTest, Operators)
	{
		Quaternion q;

		q = Quaternion(1, 2, 3, 4) + Quaternion(5, 6, 7, 8);
		EXPECT_NEAR(6, q.x(), 1e-100);
		EXPECT_NEAR(8, q.y(), 1e-100);
		EXPECT_NEAR(10, q.z(), 1e-100);
		EXPECT_NEAR(12, q.w(), 1e-100);

		q = Quaternion(8, 7, 6, 5) - Quaternion(1, 2, 3, 4);
		EXPECT_NEAR(7, q.x(), 1e-100);
		EXPECT_NEAR(5, q.y(), 1e-100);
		EXPECT_NEAR(3, q.z(), 1e-100);
		EXPECT_NEAR(1, q.w(), 1e-100);

		q = Quaternion(1, 2, 3, 4) * real_t(2);
		EXPECT_NEAR(2, q.x(), 1e-100);
		EXPECT_NEAR(4, q.y(), 1e-100);
		EXPECT_NEAR(6, q.z(), 1e-100);
		EXPECT_NEAR(8, q.w(), 1e-100);

		q = real_t(2) * Quaternion(4, 3, 2, 1);
		EXPECT_NEAR(8, q.x(), 1e-100);
		EXPECT_NEAR(6, q.y(), 1e-100);
		EXPECT_NEAR(4, q.z(), 1e-100);
		EXPECT_NEAR(2, q.w(), 1e-100);
	}

	TEST(QuaternionTest, Normalize)
	{
		EXPECT_NEAR(1, Quaternion(2, 0, 0, 0).normalize().x(), 1e-100);
		EXPECT_NEAR(1, Quaternion(0, 2, 0, 0).normalize().y(), 1e-100);
		EXPECT_NEAR(1, Quaternion(0, 0, 2, 0).normalize().z(), 1e-100);
		EXPECT_NEAR(1, Quaternion(0, 0, 0, 2).normalize().w(), 1e-100);
		EXPECT_NEAR(-1, Quaternion(-2, 0, 0, 0).normalize().x(), 1e-100);
		EXPECT_NEAR(-1, Quaternion(0, -2, 0, 0).normalize().y(), 1e-100);
		EXPECT_NEAR(-1, Quaternion(0, 0, -2, 0).normalize().z(), 1e-100);
		EXPECT_NEAR(-1, Quaternion(0, 0, 0, -2).normalize().w(), 1e-100);

		const Quaternion q = Quaternion(2, -2, 2, -2).normalize();
		EXPECT_NEAR(0.5, q.x(), 1e-100);
		EXPECT_NEAR(-0.5, q.y(), 1e-100);
		EXPECT_NEAR(0.5, q.z(), 1e-100);
		EXPECT_NEAR(-0.5, q.w(), 1e-100);
	}

	TEST(QuaternionTest, Dot)
	{
		EXPECT_NEAR(7000, Quaternion(10, 20, 30, 40).dot(Quaternion(50, 60, 70, 80)), 1e-100);
		EXPECT_NEAR(0, Quaternion(0, 0, 0, 0).dot(Quaternion(50, 60, 70, 80)), 1e-100);
		EXPECT_NEAR(0, Quaternion(10, 20, 30, 40).dot(Quaternion(0, 0, 0, 0)), 1e-100);
	}
}