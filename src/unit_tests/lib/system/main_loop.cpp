/**
 * This file is part of Rayni.
 *
 * Copyright (C) 2014-2017 Martin Ejdestig <marejde@gmail.com>
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

#include <array>
#include <chrono>
#include <cstddef>
#include <memory>
#include <thread>

#include "lib/system/linux/epoll.h"
#include "lib/system/main_loop.h"

// TODO: Remove NOLINT when https://llvm.org/bugs/show_bug.cgi?id=25594 is fixed.
using namespace std::chrono_literals; // NOLINT

namespace Rayni
{
	TEST(MainLoop, LoopAndExit)
	{
		MainLoop main_loop;

		EXPECT_FALSE(main_loop.exited());

		main_loop.exit(123);
		int exit_code = main_loop.loop();

		EXPECT_TRUE(main_loop.exited());
		EXPECT_EQ(123, exit_code);
		EXPECT_EQ(123, main_loop.exit_code());
	}

	TEST(MainLoop, ExitCodeDefaultIsSuccess)
	{
		MainLoop main_loop;

		main_loop.exit();
		int exit_code = main_loop.loop();

		EXPECT_EQ(EXIT_SUCCESS, exit_code);
		EXPECT_EQ(EXIT_SUCCESS, main_loop.exit_code());
	}

	TEST(MainLoop, ExitFromOtherThread)
	{
		MainLoop main_loop;

		// Note: May not actually test that exiting from other thread wakes up the main
		//       thread. Need to expose state (e.g. "waiting") or something to be 100% sure.
		std::thread thread([&] {
			std::this_thread::yield();
			main_loop.exit(123);
		});

		EXPECT_EQ(123, main_loop.loop());

		thread.join();
	}

	TEST(MainLoop, WaitAndDispatch)
	{
		MainLoop main_loop;
		bool called = false;

		EXPECT_FALSE(main_loop.wait(0ms));

		main_loop.run_in([&] { called = true; });

		EXPECT_TRUE(main_loop.wait());
		EXPECT_FALSE(called);

		main_loop.dispatch();
		EXPECT_TRUE(called);

		main_loop.exit();

		EXPECT_FALSE(main_loop.wait());
	}

	TEST(MainLoop, RunIn)
	{
		MainLoop main_loop;
		bool called1 = false;
		bool called2 = false;

		auto func2 = [&] {
			called2 = true;
			main_loop.exit();
		};

		main_loop.run_in([&] {
			called1 = true;
			main_loop.run_in(func2); // Must be OK to call while dispatching another callback.
		});

		main_loop.loop();

		EXPECT_TRUE(called1);
		EXPECT_TRUE(called2);
	}

	TEST(MainLoop, RunInOrderIsFIFO)
	{
		MainLoop main_loop;
		int counter = 0;
		int count1 = 0, count2 = 0;

		main_loop.run_in([&] { count1 = ++counter; });
		main_loop.run_in([&] { count2 = ++counter; });
		main_loop.run_in([&] { main_loop.exit(); });

		main_loop.loop();

		EXPECT_EQ(1, count1);
		EXPECT_EQ(2, count2);
	}

	TEST(MainLoop, RunInCallbackCalledInMainThread)
	{
		MainLoop main_loop;
		std::thread::id thread_id;
		auto func = [&] {
			thread_id = std::this_thread::get_id();
			main_loop.exit();
		};
		std::thread thread([&] { main_loop.run_in(func); });

		main_loop.loop();

		thread.join();

		EXPECT_EQ(std::this_thread::get_id(), thread_id);
	}

	TEST(MainLoop, FDPoll)
	{
		MainLoop main_loop;
		Epoll epoll;
		std::array<Epoll::Event, 1> events;

		epoll.add(main_loop.fd(), Epoll::Flag::IN);

		EXPECT_EQ(0, epoll.wait(events, 0ms));
		EXPECT_FALSE(main_loop.wait(0ms));

		main_loop.run_in([&] {});

		EXPECT_EQ(1, epoll.wait(events, 0ms));
		EXPECT_EQ(events[0].fd(), main_loop.fd());
		EXPECT_TRUE(events[0].is_set(Epoll::Flag::IN));
		EXPECT_TRUE(main_loop.wait(0ms));
	}

	TEST(MainLoop, Timer)
	{
		MainLoop main_loop;
		MainLoop::Timer timer1, timer2, timer3, timer4;
		bool called1 = false, called2 = false, called3 = false;

		timer1.start(main_loop, 1ns, [&] { called1 = true; });
		timer2.start(main_loop, MainLoop::clock::now() + 1ns, [&] { called2 = true; });
		timer3.start(main_loop, 1min, [&] { called3 = true; });
		timer4.start(main_loop, 2ns, [&] { main_loop.exit(); });

		main_loop.loop();

		EXPECT_TRUE(called1);
		EXPECT_TRUE(called2);
		EXPECT_FALSE(called3);
	}

	TEST(MainLoop, TimerRepeat)
	{
		MainLoop main_loop;
		MainLoop::Timer timer1, timer2, timer3;
		unsigned int count1 = 0, count2 = 0;

		timer1.start_repeat(main_loop, 2ns, [&] { count1++; });
		timer2.start_repeat(main_loop, 4ns, [&] { count2++; });
		timer3.start_repeat(main_loop, 20ns, [&] { main_loop.exit(); });

		main_loop.loop();

		EXPECT_LE(10, count1);
		EXPECT_LE(5, count2);
		EXPECT_GT(count1, count2); // Nothing else is certain due to first expiration "drift" when starting.
	}

	TEST(MainLoop, TimerRepeatFirstExpiration)
	{
		MainLoop main_loop;
		MainLoop::Timer timer1, timer2, timer3;
		unsigned int count1 = 0, count2 = 0;
		auto now = MainLoop::clock::now();

		timer1.start_repeat(main_loop, now + 2ns, 2ns, [&] { count1++; });
		timer2.start_repeat(main_loop, now + 3ns, 4ns, [&] { count2++; });
		timer3.start_repeat(main_loop, now + 21ns, 1h, [&] { main_loop.exit(); });

		main_loop.loop();

		EXPECT_LE(10, count1);
		EXPECT_LE(5, count2);

		auto diff = count2 - count1 / 2;
		EXPECT_TRUE(diff == 0 || diff == 1) << "count1=" << count1 << ",count2=" << count2 << ",diff=" << diff;
	}

	TEST(MainLoop, TimerRepeatStopInCallback)
	{
		MainLoop main_loop;
		MainLoop::Timer timer;
		int count = 0;

		timer.start_repeat(main_loop, 1ns, [&] {
			count++;
			if (count == 2)
			{
				timer.stop();
				main_loop.run_in([&] { main_loop.exit(); });
			}
		});

		main_loop.loop();

		EXPECT_EQ(2, count);
	}

	TEST(MainLoop, TimerStartAlreadyStarted)
	{
		MainLoop main_loop;
		MainLoop::Timer timer, delay_timer;
		bool called1 = false, called2 = false;

		timer.start(main_loop, 1min, [&] {
			called1 = true;
			main_loop.exit();
		});

		delay_timer.start(main_loop, 1ns, [&] {
			timer.start(main_loop, 1ns, [&] {
				called2 = true;
				main_loop.exit();
			});
		});

		main_loop.loop();

		EXPECT_FALSE(called1);
		EXPECT_TRUE(called2);
	}

	TEST(MainLoop, TimerStop)
	{
		MainLoop main_loop;
		MainLoop::Timer timer1, timer2;
		bool called = false;

		timer1.start(main_loop, 1ns, [&] { called = true; });
		timer2.start(main_loop, 2ns, [&] { main_loop.exit(); });
		timer1.stop();

		main_loop.loop();

		EXPECT_FALSE(called);
	}

	TEST(MainLoop, TimerCallbackNotCalledWhenDestroyed)
	{
		MainLoop main_loop;
		auto timer1 = std::make_unique<MainLoop::Timer>();
		MainLoop::Timer timer2;
		bool called = false;

		timer1->start(main_loop, 1ns, [&] { called = true; });
		timer2.start(main_loop, 2ns, [&] { main_loop.exit(); });
		timer1.reset();

		main_loop.loop();

		EXPECT_FALSE(called);
	}

	TEST(MainLoop, TimerMove)
	{
		MainLoop main_loop;
		MainLoop::Timer timer1, timer2, timer3;
		int count = 0;

		timer1.start_repeat(main_loop, 1ns, [&] {
			count++;
			if (count == 1)
			{
				timer3 = std::move(timer2);
			}
			else if (count == 2)
			{
				timer3.stop();
				main_loop.exit();
			}
		});

		timer2 = std::move(timer1);

		main_loop.loop();

		EXPECT_EQ(2, count);
	}

	TEST(MainLoop, TimerCallbackCalledInMainThread)
	{
		MainLoop main_loop;
		MainLoop::Timer timer;
		std::thread::id thread_id;
		auto func = [&] {
			thread_id = std::this_thread::get_id();
			main_loop.exit();
		};
		std::thread thread([&] { timer.start(main_loop, 1ns, func); });

		main_loop.loop();

		thread.join();

		EXPECT_EQ(std::this_thread::get_id(), thread_id);
	}

	TEST(MainLoop, TimerUsedAndDestroyedAfterMainLoop)
	{
		MainLoop::Timer timer1, timer2;
		bool called = false;

		{
			MainLoop main_loop;
			timer1.start(main_loop, 1ns, [&] { main_loop.exit(); });
			timer2.start(main_loop, 1min, [&] { called = true; });
			main_loop.loop();
		}

		timer2.stop();

		EXPECT_FALSE(called);
	}
}