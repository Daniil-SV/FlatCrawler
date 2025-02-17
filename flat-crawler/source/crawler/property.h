#pragma once

#include "crawler/schema/writer.h"
#include "core/exception/exception.h"

#include <flatbuffers/flatbuffers.h>

namespace flatCrawler
{
	class TableRoot;
	class Table;

	class Property
	{
	public:

	public:
		Property(TableRoot& root);
		~Property();

	public:
		void from_pointer(void* data);

	public:
		const flatbuffers::Table* as_table() const;
		bool is_valid_table() const;

		void guess_type();

	public:
		// Int by default
		flatbuffers::ElementaryType base_type = flatbuffers::ElementaryType::ET_INT;
		flatbuffers::SequenceType sequence_type = flatbuffers::SequenceType::ST_TABLE;
		bool is_flexbuffer = false;
		bool is_unknown_property = true;

	public:
		size_t field_size = 0;
		flatbuffers::voffset_t field_offset = 0;
		void* data = nullptr;

		TableRoot& root;
		Table* table_value = nullptr;
	};
}