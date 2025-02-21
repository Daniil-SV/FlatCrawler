#pragma once

#include "core/generic/static.h"

namespace flatCrawler
{
	class PropType
	{
	public:
		enum class Type
		{
			Unknown,
			Double,
			Float,
			Long,
			Integer,
			Short,
			Byte,
			Table,
			Struct,
			Vector,
			Flexbuffer,
			String
		};

		enum class VectorType
		{
			Invalid,
			Dynamic, // Consists of offset vectors for the required data, usually used for tables and other objects
			Fixed	 // Just an array of a fixed number of elements with a fixed value type
		};

	public:
		static const std::string ToString(Type value)
		{
			switch (value)
			{
			case Type::Unknown:
				return "int";
			case Type::Integer:
				return "uint";
			case Type::Short:
				return "ushort";
			case Type::Flexbuffer:
			case Type::Byte:
				return "ubyte";
			case Type::Double:
				return "double";
			case Type::Float:
				return "float";
			case Type::Long:
				return "long";
			case Type::String:
				return "string";
			case Type::Table:
			case Type::Struct:
			case Type::Vector:
			default:
				assert(0);
				return "";
			}
		}
	};
}