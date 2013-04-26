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

  GPLv3
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

 Apache 2.0
 Licensed under the Apache License, Version 2.0 (the "License"); 
 you may not use this file except in compliance with the License. 
 You may obtain a copy of the License at
 
      http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS, 
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and 
 limitations under the License.

USAGE

  This is a header only library, so in order to use it the only requirement is to include the header file in your source file:

#include <msgpack.hpp>

  Two main classes are provided:
    * msgpack_lite::Packer: which allows to serialize data into a MessagePack binary data stream.
    * msgpack_lite::Unpacker: that performs the deserialization from the binary stream.

   OUTPUT DATA SERIALIZATION

	In order to serialize any data, we need to declare the packer object providing the output stream object where the data will be serialized:

using namespace msgpack_lite;

Packer packer(std::cout); 

	There are several ways to serialize data using the Packer object:
  		* Using the stream operator to output serialized data:

int intValue = 0;
packer << intValue;

std::map<char, double> mapValue;
mapValue[0] = 0.0;
mapValue[1] = 1.1;
mapValue[2] = 2.2;
packer << mapValue;

  		* Using iterators to specify ranges in STL containers:

std::list<int> listValue(10, 0);
packer.pack(listValue.begin(), listValue.end());


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