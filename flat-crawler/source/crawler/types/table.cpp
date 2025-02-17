#include "table.h"
#include "crawler/root.h"

namespace flatCrawler
{
	bool Table::read()
	{
		using namespace flatbuffers;

		auto table = as_table();
		vtable = table->GetVTable();

		auto vtable_size = flatbuffers::ReadScalar<uint16_t>(vtable);
		auto vtable_data_size = flatbuffers::ReadScalar<uint16_t>(vtable + sizeof(uint16_t));
		uint8_t* vtable_end = (uint8_t*)vtable + vtable_size;

		size_t prop_count = (
			(vtable_size - 
			(sizeof(uint16_t) * 2))) // vtable size also includes its bytes, so we subtract it from there
			/ sizeof(uint16_t);		// and divide by offset size (int16) to get vtable properties count

		properties.reserve(prop_count);

		// First of all, gathering vtable offsets
		{
			voffset_t offset = 4;
			for (size_t i = 0; prop_count > i; i++)
			{
				Property& prop = properties.emplace_back(root);
				prop.field_offset = table->GetOptionalFieldOffset(offset);
				prop.from_pointer(vtable_end + prop.field_offset);
				offset += sizeof(voffset_t);
			}
		}

		// Then, we need to calculate size of each field
		for (Property& prop : properties)
		{
			if (!prop.field_offset) continue;

			// Searching for closest greather offset
			voffset_t closest_offset = 0;
			for (Property& other : properties)
			{
				if (other.field_offset > prop.field_offset && (closest_offset == 0 || other.field_offset < closest_offset)) {
					closest_offset = other.field_offset;
				}
			}

			if (closest_offset)
			{
				prop.field_size = std::abs(closest_offset - prop.field_offset);
			}
			else
			{
				prop.field_size = vtable_data_size - prop.field_offset;
			}
		}

		// and the last act, a quiz to guess the types
		for (Property& prop : properties)
		{
			prop.guess_type();
		}

		set_table_id(root.table_counter++);
		return produce_child_tables();
	}

	void Table::write_schema(SchemaWriter& writer)
	{
		// Writing children tables first
		for (Property& prop : properties)
		{
			if (prop.table_value)
			{
				prop.table_value->write_schema(writer);
			}
		}

		writer.begin_table(m_table_name);
		for (size_t i = 0; properties.size() > i; i++)
		{
			Property& prop = properties[i];
			std::string prop_name = prop.is_unknown_property ? "unused" : "prop";
			prop_name += std::to_string(i);
		
			std::string prop_typename = "";
		
			if (prop.base_type == flatbuffers::ElementaryType::ET_SEQUENCE)
			{
				switch (prop.sequence_type)
				{
				case flatbuffers::SequenceType::ST_TABLE:
				{
					if (prop.table_value)
					{
						prop_typename = prop.table_value->m_table_name;
					}
				}
				case flatbuffers::SequenceType::ST_STRUCT:
				{

				}
				default:
					break;
				}
				
			}
			else
			{
				prop_typename = SchemaWriter::GetTypename(
					prop.is_unknown_property ? 
						flatbuffers::ElementaryType::ET_INT : prop.base_type
				);
			}
		
			writer.write_property(prop_name, prop_typename);
		}
		
		writer.end_table();
	}

	bool Table::produce_child_tables()
	{
		for (Property& prop : properties)
		{
			if (prop.base_type == flatbuffers::ElementaryType::ET_SEQUENCE && prop.sequence_type == flatbuffers::SequenceType::ST_TABLE)
			{
				prop.table_value = new Table(root);
				prop.table_value->from_pointer(prop.data);
				if (!prop.table_value->read()) return false;
			}
		}

		return true;
	}

	void Table::set_table_id(uint32_t id)
	{
		m_table_id = id;
		m_table_name = "Table" + std::to_string(id);
	}
}