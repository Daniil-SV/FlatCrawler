#include "vector.h"
#include "crawler/root.h"
#include "crawler/property.h"

namespace flatCrawler
{
	Vector::Vector(TableRoot& root) : Sequence(root)
	{

	}

	Vector::Vector(Property& parent, Property& prop) : Sequence(prop)
	{
		assert(vector_type != PropType::VectorType::Invalid);
		guess_field_size(parent);
	}

	bool Vector::read()
	{
		auto vector = as_fbvector();
		length = vector->size();

		if (vector_type != PropType::VectorType::Dynamic) return true;

		Property raw_property(root);
		raw_property.field_size = field_size;

		for (size_t i = 0; length > i; i++)
		{
			uint8_t* offset = (uint8_t*)vector->data() + (i * sizeof(uint32_t));

			raw_property.from_pointer(offset);
			raw_property.guess_type();
		}

		base = from_fbtype(raw_property);
		return base->read();
	}

	void Vector::write_schema(SchemaWriter& writer)
	{
		if (base)
		{
			base->write_schema(writer);
		}
	}

	void Vector::guess_field_size(Property& parent)
	{
		if (vector_type == PropType::VectorType::Dynamic)
		{
			field_size = 4;
			return;
		}

		Sequence& seq = parent.as_sequence();
		Table& table = seq.as_table();

		for (auto& prop : table.properties)
		{

		}
	}
}