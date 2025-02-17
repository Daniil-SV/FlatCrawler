#pragma once

#include "crawler/property.h"
#include "crawler/schema/writer.h"

#include <vector>
#include <filesystem>

namespace flatCrawler
{
	class Table : public Property
	{
	public:
		using Property::Property;

	public:
		virtual bool read();
		virtual void write_schema(SchemaWriter& writer);
		bool produce_child_tables();

		void set_table_id(uint32_t id);

	public:
		bool size_prefixed = false;
		const uint8_t* vtable = nullptr;
		std::vector<Property> properties;

	protected:
		std::string m_table_name = "";
		uint32_t m_table_id = 0;
	};
}