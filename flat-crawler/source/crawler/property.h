#pragma once

#include "crawler/schema/writer.h"
#include "core/exception/exception.h"

#include <flatbuffers/flatbuffers.h>

namespace flatCrawler
{
	class TableRoot;
	class Sequence;
	class Table;
	class Struct;

	class Property
	{
	public:

	public:
		Property(TableRoot& root);
		~Property();

	public:
		void from_pointer(void* data);

	public:
		const flatbuffers::Table* as_fbtable() const;
		bool is_valid_fbtable() const;

		void guess_type();

	public:
		virtual bool is_property() const { return base_type != flatbuffers::ElementaryType::ET_SEQUENCE; }
		virtual bool is_sequence() const { return false; }

		virtual void write_schema(SchemaWriter& writer) { };

	public:
		Sequence& as_sequence();

	public:
		// Int by default
		flatbuffers::ElementaryType base_type = flatbuffers::ElementaryType::ET_INT;
		flatbuffers::SequenceType sequence_type = flatbuffers::SequenceType::ST_TABLE;
		bool is_flexbuffer = false;
		bool is_unused = true;

	public:
		size_t field_size = 0;
		flatbuffers::voffset_t field_offset = 0;
		void* data = nullptr;

		TableRoot& root;
	};
}