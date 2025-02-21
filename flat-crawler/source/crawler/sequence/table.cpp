#include "table.h"
#include "crawler/root.h"

namespace flatCrawler
{
	bool Table::read()
	{
		using namespace flatbuffers;

		auto table = as_fbtable();
		vtable = table->GetVTable();

		auto vtable_size = flatbuffers::ReadScalar<uint16_t>(vtable);
		auto vtable_data_size = flatbuffers::ReadScalar<uint16_t>(vtable + sizeof(uint16_t));
		uint8_t* vtable_end = (uint8_t*)vtable + vtable_size;

		size_t prop_count = (
			(vtable_size - 
			(sizeof(uint16_t) * 2))) // vtable size also includes its bytes, so we subtract it from there
			/ sizeof(uint16_t);		// and divide by offset size (int16) to get vtable properties count

		std::vector<Property> raw_properties;
		properties.reserve(prop_count);
		raw_properties.reserve(prop_count);

		// First of all, gathering vtable offsets
		{
			voffset_t offset = 4;
			for (size_t i = 0; prop_count > i; i++)
			{
				Property& prop = raw_properties.emplace_back(root);
				prop.field_offset = table->GetOptionalFieldOffset(offset);
				prop.from_pointer(vtable_end + prop.field_offset);
				offset += sizeof(voffset_t);
			}
		}

		// Then, we need to calculate size of each field
		for (Property& prop : raw_properties)
		{
			if (!prop.field_offset) continue;

			voffset_t begin_offset = 0;
			voffset_t end_offset = 0;
			for (Property& other : raw_properties)
			{
				if (other.field_offset < prop.field_offset && other.field_offset > begin_offset) {
					begin_offset = other.field_offset;
				}

				if (other.field_offset > prop.field_offset && (end_offset == 0 || other.field_offset < end_offset)) {
					end_offset = other.field_offset;
				}
			}

			if (end_offset)
			{
				prop.field_size = std::abs(end_offset - prop.field_offset);
			}
			else if (begin_offset)
			{
				prop.field_size = vtable_data_size - prop.field_offset;
			}
			else
			{
				prop.field_size = prop.field_offset;
			}
		}

		// and the last act, a quiz to guess the types
		for (Property& prop : raw_properties)
		{
			prop.guess_type();
		}

		// Copy raw propeties
		for (Property& prop : raw_properties)
		{
			properties.push_back(from_fbtype(prop));
		}

		return Sequence::read() && produce_childrens();
	}

	void Table::write_schema(SchemaWriter& writer)
	{
		// Calling children schema writing first
		for (auto& prop : properties)
		{
			prop->write_schema(writer);
		}

		writer.begin_table_seq(m_seq_name);
		for (size_t i = 0; properties.size() > i; i++)
		{
			auto& prop = properties[i];
			auto type = prop->get_basetype();
			std::string prop_name = type == PropType::Type::Unknown ? "unused" : "prop";
			prop_name += std::to_string(i);
		
			std::string prop_typename = prop->get_basetype_name();
			if (prop->is_vector_type())
			{
				writer.write_vector_property(prop_name, prop_typename);
			}
			else
			{
				writer.write_property(prop_name, prop_typename);
			}
		}
		
		writer.end_seq();
	}

	bool Table::produce_childrens()
	{
		for (size_t i = 0; properties.size() > i; i++)
		{
			auto& prop = properties[i];
			if (prop->is_sequence())
			{
				Sequence& seq = prop->as_sequence();
				if (!seq.read()) return false;
			}
		}

		return true;
	}
}