MsgPack-Cpp-Lite
================

MessagePack C++ lightweight header-only implementation

DESCRIPTION

  This library provides a C++ implementation of the MessagePack binary serialization protocol <http://http://msgpack.org/>   relying on an extensive usage of STL containers and stream classes.
  For more information about the format specification, please refer to <http://msgpack.org/>

AUTHOR
  
   Arturo Blas Jiménez <arturoblas@gmail.com>

VERSION
 
   0.1

LICENSE

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

USAGE

  This is a header only library, so in order to use it the only requirement is to include the header file in your source file:

#include <msgpack.hpp>

  Two main classes are provided:
    * msgpack::Packer: which allows to serialize data into a MessagePack binary data stream.
    * msgpack::Unpacker: that performs the deserialization from the binary stream.

   OUTPUT DATA SERIALIZATION

	In order to serialize any data, we need to declare the packer object providing the output stream object where the data will be serialized:

using namespace msgpack;

Packer packer(std::cout); 

	There are several ways to serialize data using the Packer object:
  		* Using the stream operator to output serialized data:

int intValue = 0;
packer << intValue;

std::map<char, double> mapValue;
map[0] = 0.0;
map[1] = 1.1;
map[2] = 2.2;
packer << mapValue;

  		* Using iterators to specify ranges in STL containers:

std::list<int> listValue(10, 0);
packer.pack(listValue.begin(), listValue.begin()+5);


	INPUT DATA DESERIALIZATION

		Firstly, the Unpacker object should be declared providing a reference to the input stream object:

Unpacker unpacker(std::cin); 

		From the receiver point of view there may be two different cases when it comes to data deserialization:
  			* The data is expected to be received in some specific order. This is the simplest case and also quite straight forward, as the formatted input operator can be used here:

float floatVal;
unpacker >> floatVal;

  			* In the second case, data is likely to be provided randomly or with no order. For this case, the Object class provides a way to get the type of the data and retrieve the value of it. In this case, the Unpacker::unpack() method should be used, which will create a new instance that should be deleted by the caller:

while(true)
{
  try
  {
    // Retrieve next object from the stream and display it
    std::auto_ptr<msgpack::Object> obj(unpacker.unpack());
    std::cout << *obj << std::endl;
  }
  catch(const msgpack::unpack_exception& e) // This is thrown when the stream end is reached
  {
    // We are done!
    break;
  }
}
