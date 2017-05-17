/**
 * This file is part of Rayni.
 *
 * Copyright (C) 2016-2017 Martin Ejdestig <marejde@gmail.com>
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

#include <fstream>
#include <string>

#include "lib/file_formats/text_reader.h"
#include "lib/system/scoped_temp_dir.h"

namespace
{
	void text_to_file(const std::string &path, const std::string &text)
	{
		std::ofstream file(path);
		file << text;
		if (!file.good())
			FAIL() << "Failed to write to file " << path << ".";
	}
}

namespace Rayni
{
	TEST(TextReader, PositionNotSet)
	{
		TextReader::Position position;
		EXPECT_EQ(0u, position.line());
		EXPECT_EQ(0u, position.column());
		EXPECT_EQ(0u, position.line_index());
	}

	TEST(TextReader, PositionNextLineAndColumn)
	{
		TextReader::Position position;

		position.next_line();
		EXPECT_EQ(1u, position.line());
		EXPECT_EQ(1u, position.column());
		EXPECT_EQ(0u, position.line_index());

		position.next_line();
		EXPECT_EQ(2u, position.line());
		EXPECT_EQ(1u, position.column());
		EXPECT_EQ(0u, position.line_index());

		position.next_column();
		EXPECT_EQ(2u, position.line());
		EXPECT_EQ(2u, position.column());
		EXPECT_EQ(1u, position.line_index());

		position.next_line();
		EXPECT_EQ(3u, position.line());
		EXPECT_EQ(1u, position.column());
		EXPECT_EQ(0u, position.line_index());

		position.next_columns(10);
		EXPECT_EQ(3u, position.line());
		EXPECT_EQ(11u, position.column());
		EXPECT_EQ(10u, position.line_index());
	}

	TEST(TextReader, PositionToString)
	{
		TextReader::Position position;
		EXPECT_EQ("", position.to_string());

		position.next_line();
		EXPECT_EQ("1:1", position.to_string());

		position.next_line();
		EXPECT_EQ("2:1", position.to_string());

		position.next_column();
		EXPECT_EQ("2:2", position.to_string());

		position.next_line();
		EXPECT_EQ("3:1", position.to_string());

		position.next_columns(10);
		EXPECT_EQ("3:11", position.to_string());

		position = TextReader::Position("prefix");
		EXPECT_EQ("prefix", position.to_string());

		position.next_line();
		EXPECT_EQ("prefix:1:1", position.to_string());
	}

	TEST(TextReader, OpenFile)
	{
		ScopedTempDir temp_dir;
		const std::string exists1_path = temp_dir.path() / "exists1.txt";
		const std::string exists2_path = temp_dir.path() / "exists2.txt";
		const std::string does_not_exist_path = temp_dir.path() / "does_not_exist.txt";

		text_to_file(exists1_path, "test1");
		text_to_file(exists2_path, "test2");

		TextReader reader;
		EXPECT_EQ("", reader.position().to_string());

		reader.open_file(exists1_path);
		EXPECT_EQ(exists1_path + ":1:1", reader.position().to_string());

		reader.open_file(exists2_path);
		EXPECT_EQ(exists2_path + ":1:1", reader.position().to_string());

		EXPECT_THROW(TextReader().open_file(does_not_exist_path), TextReader::Exception);
	}

	TEST(TextReader, SetString)
	{
		TextReader reader;
		EXPECT_EQ("", reader.position().to_string());

		reader.set_string("test1", "prefix1");
		EXPECT_EQ("prefix1:1:1", reader.position().to_string());

		reader.set_string("test2", "prefix2");
		EXPECT_EQ("prefix2:1:1", reader.position().to_string());

		reader.set_string("test3");
		EXPECT_EQ("1:1", reader.position().to_string());
	}

	TEST(TextReader, Close)
	{
		TextReader reader;
		reader.set_string("test", "prefix");
		reader.close();
		EXPECT_EQ("", reader.position().to_string());
	}

	TEST(TextReader, NextAndNextGet)
	{
		TextReader reader;
		reader.set_string("abc\ndef");

		EXPECT_EQ('a', reader.next_get());
		EXPECT_EQ('b', reader.next_get());
		EXPECT_EQ('c', reader.next_get());
		EXPECT_EQ('\n', reader.next_get());

		EXPECT_EQ('d', reader.next_get());
		reader.next();
		EXPECT_EQ('f', reader.next_get());

		EXPECT_THROW(reader.next(), TextReader::EOFException);
	}

	TEST(TextReader, At)
	{
		TextReader reader;

		EXPECT_FALSE(reader.at('a'));
		EXPECT_FALSE(reader.at_digit());
		EXPECT_FALSE(reader.at_space());
		EXPECT_FALSE(reader.at_newline());
		EXPECT_TRUE(reader.at_eof());

		reader.set_string("abc059d \t\r\ne");

		EXPECT_FALSE(reader.at_eof());

		EXPECT_TRUE(reader.at('a'));
		reader.next();
		EXPECT_FALSE(reader.at('a'));
		EXPECT_TRUE(reader.at('b'));
		reader.next();

		EXPECT_FALSE(reader.at_digit()); // 'c'
		reader.next();
		EXPECT_TRUE(reader.at_digit()); // '0'
		reader.next();
		EXPECT_TRUE(reader.at_digit()); // '5'
		reader.next();
		EXPECT_TRUE(reader.at_digit()); // '9'
		reader.next();

		EXPECT_FALSE(reader.at_space()); // 'd'
		reader.next();
		EXPECT_TRUE(reader.at_space()); // ' '
		reader.next();
		EXPECT_TRUE(reader.at_space()); // '\t'
		reader.next();
		EXPECT_TRUE(reader.at_space()); // '\r'
		EXPECT_FALSE(reader.at_newline());
		reader.next();
		EXPECT_TRUE(reader.at_space()); // '\n'
		EXPECT_TRUE(reader.at_newline());
		reader.next();

		EXPECT_TRUE(reader.at('e'));
		EXPECT_FALSE(reader.at_newline());
		EXPECT_FALSE(reader.at_eof());
		reader.next();
		EXPECT_FALSE(reader.at('e'));
		EXPECT_TRUE(reader.at_eof());
	}

	TEST(TextReader, Skip)
	{
		TextReader reader;
		reader.set_string("abcdef \t\r\n  gh  i  \njklmno p\nq");

		EXPECT_TRUE(reader.skip_char('a'));
		EXPECT_FALSE(reader.skip_char('a'));
		EXPECT_TRUE(reader.skip_char('b'));

		EXPECT_TRUE(reader.skip_string("cd"));
		EXPECT_FALSE(reader.skip_string("ee"));
		EXPECT_TRUE(reader.skip_string("ef"));

		reader.skip_space();
		EXPECT_TRUE(reader.skip_char('g'));
		reader.skip_space();
		EXPECT_TRUE(reader.skip_char('h'));

		reader.skip_space_on_line();
		EXPECT_TRUE(reader.skip_char('i'));
		reader.skip_space_on_line();
		EXPECT_TRUE(reader.skip_char('\n'));
		EXPECT_TRUE(reader.skip_char('j'));

		reader.skip_to_end_of_line();
		EXPECT_TRUE(reader.skip_char('\n'));
		EXPECT_TRUE(reader.skip_char('q'));
	}

	TEST(TextReader, Parser)
	{
		class BoolReader : public TextReader::Parser<bool>
		{
		private:
			bool parse() override
			{
				if (skip_string("true"))
					return true;
				if (skip_string("false"))
					return false;

				throw Exception(position(), "expected \"true\" or \"false\"");
			}
		};

		BoolReader reader;

		EXPECT_TRUE(reader.read_string("true"));
		EXPECT_FALSE(reader.read_string("false"));

		EXPECT_THROW(reader.read_string("abc"), BoolReader::Exception);
		EXPECT_THROW(reader.read_string(""), BoolReader::Exception);

		ScopedTempDir temp_dir;
		const std::string path = temp_dir.path() / "test.txt";
		text_to_file(path, "true");
		EXPECT_TRUE(reader.read_file(path));
	}
}
