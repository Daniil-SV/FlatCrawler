#include "property.h"

#include "crawler/root.h"

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
		if (table_value)
		{
			delete table_value;
		}
	}

	void Property::from_pointer(void* _data)
	{
		data = _data;
	}

	const flatbuffers::Table* Property::as_table() const
	{
		if (!data) return nullptr;

		return flatbuffers::GetRoot<flatbuffers::Table>(data);
	}

	bool Property::is_valid_table() const
	{
		auto table = as_table();
		flatbuffers::Verifier verifier((const uint8_t*)root.data, root.buffer.size());
		return table->VerifyTableStart(verifier);
	}

	void Property::guess_type()
	{
		if (!field_size) return;

		auto table = as_table();
		
		switch (field_size)
		{
		case 8:
			{
				// Currently only scalar types can have a field size of 8/
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
					base_type = flatbuffers::ElementaryType::ET_DOUBLE;
				}
				else
				{
					base_type = flatbuffers::ElementaryType::ET_ULONG;
				}
			}
			break;

		case 4:
			{
				uint32_t value = flatbuffers::ReadScalar<uint32_t>(data);
				
				if (is_valid_table())
				{
					base_type = flatbuffers::ElementaryType::ET_SEQUENCE;
					sequence_type = flatbuffers::SequenceType::ST_TABLE;
				}
				else
				{
					base_type = flatbuffers::ElementaryType::ET_UINT;
				}
			}
		break;

		case 2:
			{
				base_type = flatbuffers::ElementaryType::ET_USHORT;
			}
			break;
		case 1:
			{
				base_type = flatbuffers::ElementaryType::ET_UCHAR;
			}
			break;
		default:
			// if length of field is not quite usual then most likely it is a struct
			{
				base_type = flatbuffers::ElementaryType::ET_SEQUENCE;
				sequence_type = flatbuffers::SequenceType::ST_STRUCT;
			}
			break;
		}

		is_unknown_property = false;
	}
}