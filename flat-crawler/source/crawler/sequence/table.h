#pragma once

#include "crawler/property.h"
#include "crawler/sequence.h"
#include "crawler/schema/writer.h"

#include <vector>
#include <filesystem>

#include "core/memory/ref.h"

namespace flatCrawler
{
	class Table : public Sequence
	{
	public:
		using Sequence::Sequence;

	public:
		virtual bool is_table() const { return true; }
		virtual bool read() override;
		virtual void write_schema(SchemaWriter& writer);
		bool produce_childrens();

	public:
		const uint8_t* vtable = nullptr;
		std::vector<wk::Ref<Property>> properties;
	};
}