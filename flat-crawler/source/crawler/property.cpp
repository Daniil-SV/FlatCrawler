#include "property.h"

#include "crawler/root.h"
#include "is_utf8.h"

static bool is_ascii_char(unsigned char c) {
	return (c & 0x80) == 0;
}

static bool is_valid_ascii(const char* str, int len) {
	for (int i = 0; i < len; ++i) {
		unsigned char c = (unsigned char)str[i];

		if (!is_ascii_char(c)) return false;
	}

	return true;
}

namespace flatCrawler
{
	typedef union {
		float f;
		struct {
			unsigned int mantisa : 23;
			unsigned int exponent : 8;
			unsigned int sign : 1;
		} parts;
	} float_t;

	typedef union {
		double f;
		struct {
			unsigned long long mantisa : 52;
			unsigned long long exponent : 11;
			unsigned long long sign : 1;
		} parts;
	} double_t;

	Property::Property(TableRoot& _root) : root(_root)
	{
	}

	Property::~Property()
	{
	}

	wk::Ref<Property> Property::from_fbtype(Property& prop)
	{
		switch (prop.get_basetype())
		{
		case PropType::Type::Struct:
			return wk::CreateRef<Struct>(prop);
		case PropType::Type::Table:
			return wk::CreateRef<Table>(prop);
		case PropType::Type::Vector:
			return wk::CreateRef<flatCrawler::Vector>(*this, prop);
		default:
			return wk::CreateRef<Property>(prop);
		}
	}

	void Property::from_pointer(const void* _data)
	{
		data = _data;
	}

	const Property::FBTable* Property::as_fbtable(bool is_root) const
	{
		if (!data) return nullptr;
		return (const Property::FBTable*)flatbuffers::GetRoot<flatbuffers::Table>(data);
	}

	const Property::FBDVector* Property::as_fbvector() const
	{
		uint32_t offset = flatbuffers::ReadScalar<uint32_t>(data);

		return reinterpret_cast<const Property::FBDVector*>((uint8_t*)data + offset);
	}

	bool Property::is_valid_fbtable() const
	{
		using namespace flatbuffers;

		const uint8_t* buffer = (uint8_t*)root.data;
		const uint8_t* table = (uint8_t*)as_fbtable();
		
		// Check the vtable offset.
		const auto tableo = static_cast<size_t>(table - buffer);
		if (!root.verifier->Verify<soffset_t>(tableo)) return false;
		// This offset may be signed, but doing the subtraction unsigned always
		// gives the result we want.
		const auto vtableo =
			tableo - static_cast<size_t>(ReadScalar<soffset_t>(table));
		// Check the vtable size field, then check vtable fits in its entirety.
		if (!(root.verifier->VerifyComplexity() && root.verifier->Verify<voffset_t>(vtableo) &&
			root.verifier->VerifyAlignment(ReadScalar<voffset_t>(buffer + vtableo),
				sizeof(voffset_t))))
			return false;
		const auto vBuffer = buffer + vtableo;
		const auto vSize = ReadScalar<voffset_t>(vBuffer);
		const auto vDataSize = ReadScalar<voffset_t>(vBuffer + sizeof(voffset_t));
		const size_t vtable_size = (size_t)vSize + vDataSize;

		return 
			root.buffer.size() > vtable_size &&
			vSize != 0 && 
			vDataSize != 0 && 
			root.verifier->Check((vSize & 1) == 0) && 
			root.verifier->Verify(vtableo, vSize);
	}

	bool Property::is_dynamic_fbvector() const
	{
		auto vector = as_fbvector();
		
		if (!is_valid_pointer(vector)) return false;
		if (!root.verifier->VerifyVector(vector)) return false;

		auto length = vector->size();
		size_t header_size = sizeof(flatbuffers::Offset<void>) * length;

		// return false if header of assumed vector is outside file buffer
		if (!is_valid_pointer((uint8_t*)vector->data() + header_size)) return false;

		const void* last_offset = nullptr;
		for (auto it = vector->rbegin(); it.base() != vector->begin(); ++it)
		{
			const void* offset = *it;

			// TODO: not sure about this 
			if (last_offset > offset) return false;

			if (!is_valid_pointer(offset)) return false;

			last_offset = offset;
		}

		return true;
	}

	bool Property::is_valid_fbvector()
	{
		if (is_dynamic_fbvector())
		{
			vector_type = PropType::VectorType::Dynamic;
		}
		else
		{
			vector_type = PropType::VectorType::Invalid;
			return false;
		}

		return true;
	}

	bool Property::is_valid_fbstring() const
	{
		uint32_t offset = flatbuffers::ReadScalar<uint32_t>(data);
		uint8_t* string_data = (uint8_t*)data + offset;
		if (!is_valid_pointer(string_data)) return false;

		uint32_t length = flatbuffers::ReadScalar<uint32_t>(string_data);
		const char* str_begin = (char*)string_data + sizeof(uint32_t);
		const char* str_end = str_begin + length;

		if (!is_valid_pointer(str_begin, false) || !is_valid_pointer(str_end, false)) return false;
		if (!is_utf8(str_begin, length)) return false; // && !is_valid_ascii(str_begin, length)
		if (*str_end != '\0') return false;

		return true;
	}

	bool Property::is_valid_flexbuffer()
	{
		return false;
	}

	void Property::guess_type()
	{
		using flatbuffers::ElementaryType;
		if (!field_size) return;

		switch (field_size)
		{
		case 8:
			{
				// Currently only scalar types can have a field size of 8
				m_type.guess(PropType::Type::Long);

				double value = flatbuffers::ReadScalar<float>(data);
				bool valid_double = (value >= -std::numeric_limits<double>::max() && value <= std::numeric_limits<double>::max());
				if (valid_double)
				{
					flatCrawler::double_t number;
					number.f = value;

					valid_double = !(number.parts.exponent == 0x7FF && number.parts.mantisa != 0);
				}

				if (valid_double)
				{
					m_type.guess(PropType::Type::Double);
				}
			}
			break;

		case 4:
			{
				if (is_valid_fbtable())
				{
					m_type.guess(PropType::Type::Table);
				}

				if (is_valid_fbvector())
				{
					m_type.guess(PropType::Type::Vector);
				}

				if (is_valid_fbstring())
				{
					m_type.guess(PropType::Type::String);
				}

				m_type.guess(PropType::Type::Integer);
			}
		break;

		case 2:
			m_type.guess(PropType::Type::Short);
			break;
		case 1:
			m_type.guess(PropType::Type::Byte);
			break;
		default:
			// if length of field is not quite usual then most likely it is a struct
			m_type.guess(PropType::Type::Struct);
			break;
		}
	}

	bool Property::is_property_type() const
	{
		auto type = get_basetype();
		return !is_sequence_type() && !is_vector_type();
	}

	bool Property::is_sequence_type() const
	{
		auto type = get_basetype();
		return type == PropType::Type::Struct || type == PropType::Type::Table;
	}

	bool Property::is_vector_type() const
	{
		auto type = get_basetype();
		return type == PropType::Type::Vector;
	}

	Sequence& Property::as_sequence() const
	{
		if (!is_sequence()) { assert(0); };

		return *(Sequence*)this;
	}

	PropType::Type Property::get_basetype() const
	{
		return m_type.most_probable(PropType::Type::Unknown);
	}

	std::string Property::get_basetype_name() const
	{
		if (is_property_type())
		{
			return PropType::ToString(get_basetype());
			
		}
		else if (is_sequence_type())
		{
			Sequence& seq = as_sequence();
			return seq.sequence_name();
		}
		else if (is_vector_type())
		{
			Sequence& seq = as_sequence();
			Vector& vec = seq.as_vector();

			return vec.base->get_basetype_name();
		}

		assert(0);
		return "";
	}
}