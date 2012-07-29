/**
 * @file MsgPack.hpp
 * @author  Arturo Blas Jiménez <arturoblas@gmail.com>
 * @version 0.1
 *
 * @section LICENSE
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
 * @section DESCRIPTION
 *
 *  This header file provides a C++ implementation of the MessagePack
 *  binary serialization protocol <http://http://msgpack.org/> relying on
 *  an extensive usage of STL containers and stream classes.
 *
 */

#ifndef MSGPACK_HPP_
#define MSGPACK_HPP_

namespace msgpack
{
namespace bm
{
/*****************************************************
 * Type identifiers as specified by:
 * {@link http://wiki.msgpack.org/display/MSGPACK/Format+specification }
 * ***************************************************/

/*****************************************************
 * Fixed length types
 *****************************************************/

//!< Integers
static const unsigned char MP_INT8 = (unsigned char) 0xd0;
static const unsigned char MP_INT16 = (unsigned char) 0xd1;
static const unsigned char MP_INT32 = (unsigned char) 0xd2;
static const unsigned char MP_INT64 = (unsigned char) 0xd3;
static const unsigned char MP_UINT8 = (unsigned char) 0xcc;
static const unsigned char MP_UINT16 = (unsigned char) 0xcd;
static const unsigned char MP_UINT32 = (unsigned char) 0xce;
static const unsigned char MP_UINT64 = (unsigned char) 0xcf;
static const unsigned char MP_FIXNUM = (unsigned char) 0x00; //!< last 7 bits is value
static const unsigned char MP_NEGATIVE_FIXNUM = (unsigned char) 0xe0; //!< last 5 bits is value

//!< nil
static const unsigned char MP_NULL = (unsigned char) 0xc0;

//!< boolean
static const unsigned char MP_FALSE = (unsigned char) 0xc2;
static const unsigned char MP_TRUE = (unsigned char) 0xc3;

//!< Floating point
static const unsigned char MP_FLOAT = (unsigned char) 0xca;
static const unsigned char MP_DOUBLE = (unsigned char) 0xcb;

/*****************************************************
 * Variable length types
 *****************************************************/

//<! Raw bytes
static const unsigned char MP_RAW16 = (unsigned char) 0xda;
static const unsigned char MP_RAW32 = (unsigned char) 0xdb;
static const unsigned char MP_FIXRAW = (unsigned char) 0xa0; //!< last 5 bits is size

/*****************************************************
 * Container types
 *****************************************************/

//!< Arrays
static const unsigned char MP_ARRAY16 = (unsigned char) 0xdc;
static const unsigned char MP_ARRAY32 = (unsigned char) 0xdd;
static const unsigned char MP_FIXARRAY = (unsigned char) 0x90; //<! last 4 bits is size

//!< Maps
static const unsigned char MP_MAP16 = (unsigned char) 0xde;
static const unsigned char MP_MAP32 = (unsigned char) 0xdf;
static const unsigned char MP_FIXMAP = (unsigned char) 0x80; //last 4 bits is size

// Some helper bitmasks
static const int MAX_4BIT = 0xf;
static const int MAX_5BIT = 0x1f;
static const int MAX_7BIT = 0x7f;
static const int MAX_8BIT = 0xff;
static const int MAX_15BIT = 0x7fff;
static const int MAX_16BIT = 0xffff;
static const int MAX_31BIT = 0x7fffffff;
static const long MAX_32BIT = 0xffffffffL;

} //namespace bm

/**
 * Packer class. Provides the functionality to serialize data
 * into a stream using the MessagePack binary data.
 */
class Packer
{
public:

	/**
	 * Constructor
	 * @param out Packer output stream where the binary data will be put
	 */
	explicit Packer(std::ostream& out) : out_(out) {}

	/**
	 * Packs using a pointer as the data source. If the pointer is null
	 * a null object will be serialized into the stream, if not,
	 * the contents of the pointer will be serialized.
	 * @item Pointer to the data to be serialized.
	 */
	template<class T> inline
	Packer& pack(const T* item)
	{
		return (item == 0) ? write(bm::MP_NULL) : pack(*item);
	}

	/**
	* Packs using a C string as a source of data. If the pointer is null
	* a null object will be serialized into the stream, if not,
    * the contents of the array will be serialized using the lenght of
    * the string provided by strlen(). Note that this is not suitable for
    * serializing raw regions of memory, in order to do so,
    * {@see #Packer::pack(const char*, std::size) } should be used instead.
	* @item Pointer to the C string to be serialized.
    */
	inline
	Packer& pack(const char* item)
	{
		return (item == 0) ? write(bm::MP_NULL) : pack(item, strlen(item));
	}

	/**
	 * Pack a boolean value
	 * @item Value to be serialized
	 */
	inline Packer& pack(bool item)
	{
		return (item ? write(bm::MP_TRUE) : write(bm::MP_FALSE));
	}

	/**
	 * Pack an integer value
	 * @item Value to be serialized
	 */
	inline
	Packer& pack(int item)
	{
		return pack((long) item);
	}

	/**
	* Pack a long integer value
	* @item Value to be serialized
    */
	inline
	Packer& pack(long value)
	{
		if (value >= 0)
		{
			if (value <= bm::MAX_7BIT)
			{
				write<char>((char(value) | bm::MP_FIXNUM));
			}
			else if (value <= bm::MAX_8BIT)
			{
				write(bm::MP_UINT8).write<char>(value);
			}
			else if (value <= bm::MAX_16BIT)
			{
				write(bm::MP_UINT16).write<short>(value);
			}
			else if (value <= bm::MAX_32BIT)
			{
				write(bm::MP_UINT32).write<int>(value);
			}
			else
			{
				write(bm::MP_UINT64).write<long>(value);
			}
		}
		else
		{
			if (value >= -(bm::MAX_5BIT + 1))
			{
				write<int>(bm::MAX_8BIT & int(value));
			}
			else if (value >= -(bm::MAX_7BIT + 1))
			{
				write(bm::MP_INT8).write<char>(value);
			}
			else if (value >= -(bm::MAX_15BIT + 1))
			{
				write(bm::MP_INT16).write<short>(value);
			}
			else if (value >= -(long(bm::MAX_31BIT) + 1))
			{
				write(bm::MP_INT32).write<int>(value);
			}
			else
			{
				write(bm::MP_INT64).write<long>(value);
			}
		}
		return *this;
	}

	/**
	* Pack a single precision floating point value
	* @item Value to be serialized
    */
	inline
	Packer& pack(float item)
	{
		return write(bm::MP_FLOAT).write(item);
	}

	/**
	* Pack a double precision floating point value
	* @item Value to be serialized
    */
	inline
	Packer& pack(double item)
	{
		return write(bm::MP_DOUBLE).write(item);
	}


	inline
	Packer& pack(const std::string& value)
	{
		return pack(value.c_str(), value.size());
	}

	inline
	Packer& pack(const char* data, std::size_t length)
	{
		if (length <= bm::MAX_5BIT)
		{
			write((char) (((char) length) | bm::MP_FIXRAW));
		}
		else if (length <= bm::MAX_16BIT)
		{
			write(bm::MP_RAW16).write<short>(length);
		}
		else
		{
			write(bm::MP_RAW32).write<int>(length);
		}
		out_.write(data, length);
		return *this;
	}

	template<class IteratorT> inline
	Packer& pack(IteratorT first, IteratorT last)
	{
		typename std::iterator_traits<IteratorT>::value_type type;

		std::size_t length = std::distance(first, last);
		initContainer(length, type);

		// perform function for each element
		for (; first != last; ++first)
			pack(*first);

		return *this;
	}


	template<class keyT, class valT> inline
	Packer& pack(const std::pair<const keyT, valT>& item)
	{
		return pack(item.first).pack(item.second);
	}

	template<typename T, typename U> inline
	Packer& pack(const std::map<T, U>& item)
	{
		return pack(item.begin(), item.end());
	}

private:

	template<typename T> inline
	Packer& write(T data)
	{
		out_.write((char*) &data, sizeof(T));
		return *this;
	}

	template<class T> inline
	void initContainer(std::size_t& length, T&)
	{
		if (length <= bm::MAX_4BIT)
		{
			write<char>(((char) length) | bm::MP_FIXARRAY);
		}
		else if (length <= bm::MAX_16BIT)
		{
			write(bm::MP_ARRAY16).write<short>(length);
		}
		else
		{
			write(bm::MP_ARRAY32).write<int>(length);
		}
	}

	template<class key, class val> inline
	void initContainer(std::size_t& length, std::pair<const val, key>&)
	{
		if (length <= bm::MAX_4BIT)
		{
			write<char>(((char) length) | bm::MP_FIXMAP);
		}
		else if (length <= bm::MAX_16BIT)
		{
			write(bm::MP_MAP16).write<short>(length);
		}
		else
		{
			write(bm::MP_MAP32).write<int>(length);
		}
	}

	std::ostream& out_; //!< The output stream where the data is packed in

}; // Packer

/**
 * Type enum type defines all the possible output types
 * for the {@see #Object} instances generated by
 * the {@see #Unpacker} class
 */
enum object_type
{
	NIL, 		//<! nil
	BOOLEAN,    //<! boolean [true, false]
	CHAR,		//<! int8 [positive, negative fixnum]
	SHORT,		//<! int16
	INTEGER,	//<! int32
	LONG,		//<! int64
	UCHAR,		//<! uint8
	USHORT,		//<! uint16
	UINTEGER,	//<! uint32
	ULONG,		//<! uint64
	FLOAT,		//<! float
	DOUBLE,		//<! double
	RAW,		//<! Raw bytes [fix raw, raw 16, raw 32]
	ARRAY,		//<! Array [fix array, array 16, array 32]
	MAP,		//<! Map [fix map, map 16, map 32]

}; // object_type

namespace detail
{

template<object_type t>
struct type_traits {
};

// Forward definition
template<object_type type>
class ObjectImpl;

} // namespace detail

/**
 * Object class. Represents a piece of deserialized
 * data, and provides access to the data itself.
 */
class Object
{
public:

	/**
	 * Destructor
	 */
	virtual ~Object() {}

	/**
	 * Retrieve the object type for the
	 * current Object.
	 */
	object_type getType() const
	{
		return type_;
	}

	/**
	 * Return the current Object data as the specified
	 * type provided as a template parameter.
	 * In case the provided type is not current Object's type
	 * a std::bad_cast exception is thrown, otherwise, the
	 * method returns a reference to the inner data.
	 */
	template<object_type type>
	typename detail::type_traits<type>::type& getAs() const throw(std::bad_cast)
	{
		if(type!=type_)
			throw std::bad_cast();

		return getAs<typename detail::type_traits<type>::type>();
	}

	/**
	 * Return the current Object data as the specified type
	 * provided as a template parameter.
	 * Note that this method will always return a reference, as the cast
	 * is forced to the given type.
	 */
	template<class T>
	T& getAs() const
	{
		return *(T*)(this->get());
	}

	/**
	 * Returns true if this object represents
	 * a nil instance.
	 */
	virtual bool isNil() const
	{
		return true;
	}

protected:

   /**
    * This method returns a pointer to the
    * inner data. It is overloaded in
    * the specific implementations.
    */
	virtual void* get() const
	{
		return 0;
	}

	/**
	 * Protected constructor. This is a virtual class.
	 * @param type Type handled by the current object instance.
	 */
	explicit Object(object_type type) : type_(type) {}

private:
	const object_type type_; //!< Type of the current object

}; // Object

/**
 * Nil class. Represents a nil object.
 */
class Nil: public Object
{
public:

	Nil() : Object(NIL) {} //<! Constructor

	bool isNil() const
	{
		// I'm the nil guy!
		return true;
	}
};

namespace detail
{

/**
 * Class ObjectImpl. Internal implementation for the speific Object types
 * that handle fixed length types, except for nil.
 */
template<object_type type>
class ObjectImpl: public Object
{
public:
	typedef typename detail::type_traits<type>::type T; //<! My type

	/**
	 * Constructor.
	 * @param value Value contained by the Object. There's a implicit
	 * copy performed here.
	 */
	explicit ObjectImpl(const T& value) : Object(type), value_(value) {}

	/**
	 * Cast operator
	 */
	operator T() const
	{
		return value_;
	}

private:

	// From Object
	void* get() const
	{
		// Return own data value
		return (void*) &value_;
	}

	T value_; //< Instance of the data handled by the Object

}; // ObjectIml

} // namespace detail

namespace detail
{

/**
 * Declaration of the types correspondences with
 * their specific data types
 */

template<>
struct type_traits<BOOLEAN>
{
	typedef bool type;
};

template<>
struct type_traits<CHAR>
{
	typedef char type;
};

template<>
struct type_traits<SHORT>
{
	typedef short type;
};

template<>
struct type_traits<INTEGER>
{
	typedef int type;
};

template<>
struct type_traits<LONG>
{
	typedef short type;
};

template<>
struct type_traits<UCHAR>
{
	typedef unsigned char type;
};

template<>
struct type_traits<USHORT>
{
	typedef unsigned short type;
};

template<>
struct type_traits<UINTEGER>
{
	typedef unsigned int type;
};

template<>
struct type_traits<ULONG>
{
	typedef unsigned short type;
};

template<>
struct type_traits<FLOAT>
{
	typedef float type;
};

template<>
struct type_traits<DOUBLE>
{
	typedef double type;
};

template<>
struct type_traits<RAW>
{
	typedef std::string type;
};

template<>
struct type_traits<ARRAY>
{
	typedef std::list<Object*> type;
};

template<>
struct type_traits<MAP>
{
	typedef std::multimap<Object*,Object*> type;
};

/**
 * ObjectImpl class specilization for the ARRAY type.
 */
template<>
class ObjectImpl<ARRAY>: public Object
{
public:
	typedef type_traits<ARRAY>::type array_t; //!< Own array data type

	/**
	 * Destructor. Removes and deletes all the Objects
	 * contained in the array.
	 */
	~ObjectImpl()
	{
		array_t::iterator it = value_.begin();;
		while(it!=value_.end())
		{
			delete *it;
			*it = 0;
			it++;
		}
		value_.clear();
	}

	ObjectImpl() : Object(ARRAY) {} //!< Constructor

	/**
	 * Cast operator
	 */
	operator array_t() const
	{
		return value_;
	}

	/**
	 * Add a new object to the array, which
	 * will be owned and deleted by it on destruction.
	 */
	void add(Object* o)
	{
		value_.push_back(o);
	}

protected:

	// From Object
	void* get() const
	{
		return (void*) &value_;
	}

private:
	array_t value_; //!< Instance of the array handled by the Object
};

/**
 * ObjectImpl class specilization for the MAP type.
 */
template<>
class ObjectImpl<MAP>: public Object
{
public:
	typedef type_traits<MAP>::type map_t; //!< Own map data type

	/**
    * Destructor. Removes and deletes all the Object keys
	* and values contained in the map.
	*/
	~ObjectImpl()
	{
		map_t::iterator it = value_.begin();;
		while(it!=value_.end())
		{
			delete it->first;
			delete it->second;
			it++;
		}
		value_.clear();
	}

	ObjectImpl() : Object(MAP) {} //!< Constructor

	/**
	 * Cast operator
	 */
	operator map_t() const
	{
		return value_;
	}

	/**
	 * Insert a new pair key-value into the map, which
	 * will be owned and deleted by it on destruction.
	 */
	void insert(Object* key, Object* val)
	{
		value_.insert(map_t::value_type(key, val));
	}

protected:

	// From Object
	void* get() const
	{
		return (void*) &value_;
	}

private:
	map_t value_; //!< Instance of the map handled by the Object
};

/**
 * ObjectImpl class specilization for the RAW type.
 */
template<>
class ObjectImpl<RAW>: public Object
{
public:
	typedef type_traits<RAW>::type raw_t; //!< Own raw data type

	/**
	 * Constructor
	 * @value Reference to the data region handled
	 * by the Object, which ownership is now from
	 * the current Object which will destroy it on
	 * deletion.
	 */
	ObjectImpl(raw_t& value) : value_(&value), Object(RAW)
	{
	}

	/**
	 * Destructor
	 */
	~ObjectImpl()
	{
		delete value_;
	}

	/**
	 * Cast operator
	 */
	operator raw_t() const
	{
		return *value_;
	}

protected:

	// From Object
	void* get() const
	{
		return (void*) value_;
	}

private:
	raw_t* value_; //!< Pointer to the instance of the buffer handled by the Object
};

} // namespace detail

typedef detail::ObjectImpl<BOOLEAN> Bool;
typedef detail::ObjectImpl<CHAR> Char;
typedef detail::ObjectImpl<SHORT> Short;
typedef detail::ObjectImpl<INTEGER> Int;
typedef detail::ObjectImpl<LONG> Long;
typedef detail::ObjectImpl<UCHAR> UnsignedChar;
typedef detail::ObjectImpl<USHORT> UnsignedShort;
typedef detail::ObjectImpl<UINTEGER> UnsignedInt;
typedef detail::ObjectImpl<ULONG> UnsignedLong;
typedef detail::ObjectImpl<FLOAT> Float;
typedef detail::ObjectImpl<DOUBLE> Double;
typedef detail::ObjectImpl<RAW> Raw;
typedef detail::ObjectImpl<ARRAY> Array;
typedef detail::ObjectImpl<MAP> Map;

/**
 * Exception likely to be thrown during the
 * data deserialization.
 */
typedef std::ios_base::failure unpack_exception;

/**
 * Unpacker class. Allows to deserialize MessagePack binary data
 * from a stream
 */
class Unpacker
{
public:

	/**
	* Constructor
    * @param in Unpacker input stream from where the binary data is taken
    */
	Unpacker(std::istream& in) : in_(in) {}

	/**
	 * Constructs an Object from the available data in the input
	 * stream. The caller is responsible for deleting the returned
	 * instance.
	 * This method may throw an {@see #unpack_exception} in case the
	 * buffer runs out of data while trying to deserialize.
	 */
	Object* unpack() throw(unpack_exception)
	{
		if(in_.eof())
			throw unpack_exception("Reached end of stream");

		unsigned char value;
		read(value); // Read the header

		int size;
		float fVal;
		double dVal;
		char cVal;
		short sVal;
		int iVal;
		long lVal;
		unsigned char ucVal;
		unsigned short usVal;
		unsigned int uiVal;
		unsigned long ulVal;

		switch (value)
		{
		case bm::MP_NULL:
			return new Nil();
		case bm::MP_FALSE:
			return new Bool(false);
		case bm::MP_TRUE:
			return new Bool(true);
		case bm::MP_FLOAT:
			read(fVal);
			return new Float(fVal);
		case bm::MP_DOUBLE:
			read(dVal);
			return new Double(dVal);
		case bm::MP_UINT8:
			read(ucVal);
			return new UnsignedChar(ucVal);
		case bm::MP_UINT16:
			read(usVal);
			return new UnsignedShort(usVal);
		case bm::MP_UINT32:
			read(uiVal);
			return new UnsignedInt(uiVal);
		case bm::MP_UINT64:
			read(ulVal);
			return new UnsignedLong(ulVal);
		case bm::MP_INT8:
			read(cVal);
			return new Char(cVal);
		case bm::MP_INT16:
			read(sVal);
			return new Short(sVal);
		case bm::MP_INT32:
			read(uiVal);
			return new Int(iVal);
		case bm::MP_INT64:
			read(lVal);
			return new Long(lVal);
		case bm::MP_ARRAY16:
			read(sVal);
			return unpackList(sVal);
		case bm::MP_ARRAY32:
			read(iVal);
			return unpackList(iVal);
		case bm::MP_MAP16:
			read(sVal);
			return unpackMap(sVal);
		case bm::MP_MAP32:
			read(iVal);
			return unpackMap(iVal);
		case bm::MP_RAW16:
			read(sVal);
			return unpackRaw(sVal);
		case bm::MP_RAW32:
			read(iVal);
			return unpackRaw(iVal);
		}

		if (((unsigned char)(value & 0xF0)) == bm::MP_FIXRAW)
		{
			return unpackRaw(value & 0x0F);
		}

		if (((unsigned char)(value & 0xE0)) == bm::MP_NEGATIVE_FIXNUM)
		{
			return new Int(value & 0x0F);
		}

		if (((unsigned char)(value & 0xD0)) == bm::MP_FIXARRAY)
		{
			return unpackList(value & 0x2F);
		}

		if (((unsigned char)(value & 0xD0)) == bm::MP_FIXMAP)
		{
			return unpackMap(value & 0x2F);
		}

		if (value <= 127) //MP_FIXNUM
		{
			return new Int(value);
		}
		else
		{
			return 0;
		}
	}

private:

	Object* unpackList(int size)
	{
		if (size < 0)
			return 0;

		Array* ret = new Array();
		for (int i = 0; i < size; ++i)
		{
			ret->add(unpack());
		}
		return ret;
	}

	Object* unpackMap(int size)
	{
		if (size < 0)
			return 0;

		Map* ret = new Map();

		for (int i = 0; i < size; ++i)
		{
			Object* key = unpack();
			Object* val = unpack();
			ret->insert(key, val);
		}
		return ret;
	}

	Object* unpackRaw(int size)
	{
		if (size < 0)
			return 0;

		detail::type_traits<RAW>::type* data = new detail::type_traits<RAW>::type();
		data->resize(size);

		in_.read(&*data->begin(), size);

		return new Raw(*data);
	}

	template<typename T> inline
	Unpacker& read(T& ret) throw(unpack_exception)
	{
		if(in_.readsome((char*) &ret, sizeof(T))<sizeof(T))
			throw unpack_exception("Reached end of stream while reading");

		return *this;
	}

	std::istream& in_; //!< The stream we are unpacking the data from
};

} // namespace msgpack

/**
 * Serialize the given instance of the template type T into
 * a Packer object.
 */
template<typename T> inline
msgpack::Packer& operator<<(msgpack::Packer& p, const T& v)
{
	return p.pack(v);
}

/**
 * Deserialize from the provided Unpacker object forcing a cast into the
 * provided object reference. Unpacking results are copied into v, so
 * this is not recommended when deserializing arrays, maps or raw object
 * types is expected.
 * This may throw an exception if the Unpacker runs out of input data.
 */
template<typename T> inline
msgpack::Unpacker& operator>>(msgpack::Unpacker& u, T& v) throw (msgpack::unpack_exception)
{
	msgpack::Object* obj = 0;
	if((obj = u.unpack())!=0)
	{
		v = obj->getAs<T>();
	}
	else
	{
		// TODO: rollback unpacker action?
		throw msgpack::unpack_exception("Unable to get object from stream");
	}
	delete obj;
}

/**
 * Print an Object instance into the provided output stream.
 */
std::ostream& operator<<(std::ostream& s, const msgpack::Object& o)
{
	using namespace msgpack;

	detail::type_traits<ARRAY>::type::iterator arrayIt;
	detail::type_traits<MAP>::type::iterator mapIt;

	switch (o.getType())
	{
	case NIL:
			return (s << "null");
	case BOOLEAN:
			return (s << (o.getAs<BOOLEAN>()? "true" : "false"));
	case CHAR:
		return (s << o.getAs<CHAR>());
	case SHORT:
		return (s << o.getAs<SHORT>());
	case INTEGER:
		return (s << o.getAs<INTEGER>());
	case LONG:
		return (s << o.getAs<LONG>());
	case UCHAR:
		return (s << o.getAs<UCHAR>());
	case USHORT:
		return (s << o.getAs<USHORT>());
	case UINTEGER:
		return (s << o.getAs<UINTEGER>());
	case ULONG:
		return (s << o.getAs<ULONG>());
	case FLOAT:
		return (s << o.getAs<FLOAT>());
	case DOUBLE:
		return (s << o.getAs<DOUBLE>());
	case RAW:
		return (s << '"' << o.getAs<RAW>() << '"');
	case ARRAY:
		arrayIt = o.getAs<ARRAY>().begin();
		s << "array{";
		while(++arrayIt!=o.getAs<ARRAY>().end())
		{
			s << "[" << **arrayIt++ << "]";
		}
		s << "}";
		break;
	case MAP:
		mapIt = o.getAs<MAP>().begin();
		s << "map{";
		while(mapIt!=o.getAs<MAP>().end())
		{
			s << "[" << *mapIt->first << "," << *mapIt->second << "]";
			mapIt++;
		}
		s << "}";
		break;
	default:
		return (s << "[Unknown type]");
	}
	return s;
}

#endif // MSGPACK_HPP_
