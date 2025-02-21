#pragma once
#include "crawler/sequence.h"
#include "core/memory/ref.h"

namespace flatCrawler
{
	class Property;
	class TableRoot;
	class Table;

	class Vector : public Sequence
	{
	public:
		Vector(TableRoot& root);
		Vector(Property& parent, Property& prop);

	public:
		virtual bool is_vector() const { return true; }
		virtual bool read();
		virtual void write_schema(SchemaWriter& writer);
		void guess_field_size(Property& parent);

	public:
		size_t length = 0;
		wk::Ref<Property> base;
	};
}