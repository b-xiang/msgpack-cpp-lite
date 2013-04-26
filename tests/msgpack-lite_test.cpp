/**
 * @file msgpack-lite_test.hpp
 * @author  Arturo Blas Jiménez <arturoblas@gmail.com>
 * @version 0.1
 *
 * @section LICENSE
 *
 * \GPLv3
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * \Apache 2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 *
 */

#include "msgpack/msgpack-lite.hpp"

#include <gtest/gtest.h>

#include <sstream>
#include <limits>

using testing::Types;

/*
 --------------------------------------------------------------------
 UNIT TESTS
 --------------------------------------------------------------------
 */

using namespace msgpack_lite;

struct get_printable {
	template<typename T> inline const T& operator()(const T& t) {
		return t;
	}

	inline std::string operator()(const std::wstring& t) {
		std::string str;
		str.assign(t.begin(), t.end());
		return str;
	}
};

template<typename T>
class TestHelper
{
public:
	TestHelper() :
			packer_(ss_), unpacker_(ss_) {
	}

	virtual void operator()() = 0;

protected:

	virtual bool test(const T&input, T& output) = 0;

	Packer packer_;
	Unpacker unpacker_;

	bool run(const T& input) {
		T dest;
		bool equal = test(input, dest);
		if (!equal) {
			std::cerr << "Error testing input " << get_printable()(input)
					<< "-->" << get_printable()(dest) << std::endl;
		}

		ss_.str().clear();

		return equal;
	}

	std::stringstream ss_;
};

//////////////////////////////////////////////////////////////////////

template<typename T, int samples = 2048>
class RangeTestHelper: public TestHelper<T>
{
public:

	void operator()() {
		typedef std::numeric_limits<T> limits;

		long double min = limits::min(), max = limits::max();
		long double step = ceil((max - min) / (samples));

		EXPECT_TRUE(this->run((T) (min)));
		EXPECT_TRUE(this->run((T)(max)));
		EXPECT_TRUE(this->run((T)(0)));

		for (long double i = min; i < max; i += step) {
			bool ret = this->run((T) (i));

			EXPECT_TRUE(ret);

			if (!ret)
				break;
		}
	}
};

//////////////////////////////////////////////////////////////////////


template<typename T>
class TestRange: public testing::Test {
};

// The list of types we want to test.
typedef Types<bool, char, short, int, long, unsigned char, unsigned short,
		unsigned int, unsigned long, int8_t, int16_t, int32_t, int64_t, uint8_t,
		uint16_t, uint32_t, uint64_t, float, double> TestTypes;


TYPED_TEST_CASE(TestRange, TestTypes);

//////////////////////////////////////////////////////////////////////

TYPED_TEST(TestRange, stream_operator)
{
	class StreamOpRangeTestHelper :
		public RangeTestHelper<TypeParam>
	{
	protected:

		bool test(const TypeParam&input, TypeParam& output)
		{
			this->packer_ << input;
			this->unpacker_ >> output;
			return input == output;
		}
	};

	StreamOpRangeTestHelper test;
	test();
}

TYPED_TEST(TestRange, pack_unpack)
{
	class PackUnpackRangeTestHelper :
		public RangeTestHelper<TypeParam>
	{
	protected:

		bool test(const TypeParam&input, TypeParam& output)
		{
			this->packer_.pack(input);
			this->unpacker_.unpack(output);
			return input == output;
		}
	};

	PackUnpackRangeTestHelper test;
	test();
}

//////////////////////////////////////////////////////////////////////

template<typename T>
class StringTestHelper: public TestHelper<T>
{
public:

	void operator()() {
		for (std::size_t i = 0; i < testStrings_.size(); i++) {
			T str;
			str.assign(testStrings_[i].begin(), testStrings_[i].end());
			bool ret = this->run(str);
			EXPECT_TRUE(ret);
			if (!ret)
				break;
		}
	}

protected:

	virtual void SetUp()
	{
		testStrings_.push_back("");
		testStrings_.push_back("a");
		testStrings_.push_back("com.uoa.cs.test");
		testStrings_.push_back("\n\t\testtest");
		testStrings_.push_back("@#$@#&*^*('");
	}

	// virtual void TearDown() {}

	std::vector<std::string> testStrings_;
};

//////////////////////////////////////////////////////////////////////

template<typename T>
class TestStrings: public testing::Test {
};

typedef Types<std::string, std::wstring> StringTypes;

TYPED_TEST_CASE(TestStrings, StringTypes);

//////////////////////////////////////////////////////////////////////

TYPED_TEST(TestStrings, stream_operator)
{
	class StreamOpStringTestHelper :
		public StringTestHelper<TypeParam>
	{
	protected:

		bool test(const TypeParam&input, TypeParam& output)
		{
			this->packer_ << input;
			this->unpacker_ >> output;
			return input == output;
		}
	};

	StreamOpStringTestHelper test;
	test();
}

TYPED_TEST(TestStrings, pack_unpack)
{
	class PackUnpackStringTestHelper :
		public StringTestHelper<TypeParam>
	{
	protected:

		bool test(const TypeParam&input, TypeParam& output)
		{
			this->packer_.pack(input);
			this->unpacker_.unpack(output);
			return input == output;
		}
	};

	PackUnpackStringTestHelper test;
	test();
}

// TODO: test pointers, parcelable, array and maps

TEST(Examples, example1)
{
Packer packer(std::cout);
int intValue = 0;
packer << intValue;

std::map<char, double> mapValue;
mapValue[0] = 0.0;
mapValue[1] = 1.1;
mapValue[2] = 2.2;
packer << mapValue;
}

TEST(Examples, example2)
{
Packer packer(std::cout);
std::list<int> listValue(10, 0);
packer.pack(listValue.begin(), listValue.end());
}

TEST(Examples, example3)
{
	std::stringstream ss;
	Unpacker unpacker(ss);
	float floatVal;
	unpacker >> floatVal;
}

TEST(Examples, example4)
{
	std::stringstream ss;
	Unpacker unpacker(ss);
	while(true)
	{
	  try
	  {
	    // Retrieve next object from the stream and display it
	    Object* obj = unpacker.unpack();
	    if(obj)
	    {
	    	switch(obj->getType())
	    	{
	    	// Do stuff here
	    	}
	    	delete obj;
	    }
	  }
	  catch(const unpack_exception& e) // This is thrown when the stream end is reached
	  {
	    // We are done!
	    break;
	  }
	}
}
