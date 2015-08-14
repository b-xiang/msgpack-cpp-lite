### This library provides a C++ implementation of the MessagePack binary data serialization ###

**For more information about the format specification, please refer to [msgpack.org](http://msgpack.org)**

The design encourages simplicity of use and efficiency through zero-copy, making an extensive use of STL containers and streams.

This is a header only library, so in order to use it the only requirement is to include the header file in your source file:
```
#include <msgpack/msgpack-lite.hpp>
```

Two main classes are provided:
  * **`msgpack_lite::Packer`**: which allows to serialize data into a MessagePack binary data stream.
  * **`msgpack_lite::Unpacker`**: that performs the deserialization from the binary stream.

### Output data serialization ###
In order to serialize any data, we need to declare the packer object providing the output stream object where the data will be serialized:
```
using namespace msgpack_lite;

Packer packer(std::cout); 
```
There are several ways to serialize data using the `Packer` object:
  * Using the stream operator to output serialized data:
```
int intValue = 0;

packer << intValue;

float floatVal = 1.0f;
std::string strVal("Hello");
short shortVal = -12;
char* null = 0;

packer << floatVal << strVal << "world!" << shortVal << null << false;

std::map<char, double> mapVal;
mapVal[0] = 0.0;
mapVal[1] = 1.1;
mapVal[2] = 2.2;

packer << mapVal;
```
  * Using iterators to specify ranges in STL containers:
```
std::list<int> listValue(10, 0);
packer.pack(listValue.begin(), listValue.end());
```

### Input data deserialization ###
Firstly, the `unpacker` object should be declared providing a reference to the input stream object:
```
Unpacker unpacker(std::cin); 
```
From the receiver point of view there may be two different cases when it comes to data deserialization:
  * The data is expected to be received in some specific order. This is the simplest case and also quite straight forward, as the formatted input operator can be used here:
```
float floatVal;

unpacker >> floatVal;

bool boolVal;
str::string strVal;
short shortVal;

unpacker >> boolVal >> strVal >> shortVal;
```
  * In the second case, data is likely to be provided randomly or with no order. For this case, the `Object` class provides a way to get the type of the data and retrieve the value of it. In this case, the `Unpacker::unpack()` method should be used, which will create a new instance that should be deleted by the caller:
```
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
```

### License ###
The code comes under the terms of a dual Apache2.0+GPLv3 license.