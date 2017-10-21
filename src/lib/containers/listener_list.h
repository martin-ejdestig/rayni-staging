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

#ifndef RAYNI_LIB_CONTAINERS_LISTENER_LIST_H
#define RAYNI_LIB_CONTAINERS_LISTENER_LIST_H

#include <algorithm>
#include <type_traits>
#include <vector>

namespace Rayni
{
	template <typename Listener>
	class ListenerList
	{
	public:
		class ListenerBase
		{
			friend class ListenerList<Listener>;

		protected:
			ListenerBase() = default;

			ListenerBase(const ListenerBase &other) = delete; // TODO: Make copyable... ?
			ListenerBase(ListenerBase &&other) = delete; // TODO: Make noexcept moveable

			virtual ~ListenerBase()
			{
				if (list_)
					// TODO: Unsafe cast (but will work for most common use case).
					list_->remove(*static_cast<Listener *>(this));
			}

			ListenerBase &operator=(const ListenerBase &other) = delete; // TODO: Make copyable... ?
			ListenerBase &operator=(ListenerBase &&other) = delete; // TODO: Make noexcept moveable

		private:
			ListenerList<Listener> *list_ = nullptr;
		};

		ListenerList()
		{
			static_assert(std::is_base_of<ListenerBase, Listener>::value,
			              "Listener must inherit from ListenerBase");
		}

		ListenerList(const ListenerList &other) = delete; // TODO: Make copyable... ?
		ListenerList(ListenerList &&other) = delete; // TODO: Make noexcept moveable

		~ListenerList()
		{
			for (auto listener : listeners_)
				listener->list_ = nullptr;
		}

		ListenerList &operator=(const ListenerList &other) = delete; // TODO: Make copyable... ?
		ListenerList &operator=(ListenerList &&other) = delete; // TODO: Make noexcept moveable

		void add(Listener &listener)
		{
			if (listener.list_)
				listener.list_->remove(listener);

			listeners_.emplace_back(&listener);
			listener.list_ = this;
		}

		void remove(Listener &listener)
		{
			auto iterator = std::find(listeners_.cbegin(), listeners_.cend(), &listener);
			if (iterator != listeners_.cend())
			{
				listeners_.erase(iterator);
				listener.list_ = nullptr;
			}
		}

		template <typename Method, typename... Args>
		void notify(Method method, const Args &... args)
		{
			for (auto listener : listeners_)
				(listener->*method)(args...);
		}

	private:
		std::vector<Listener *> listeners_;
	};
}

#endif // RAYNI_LIB_CONTAINERS_LISTENER_LIST_H
