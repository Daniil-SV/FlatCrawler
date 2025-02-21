#pragma once

#include "crawler/schema/writer.h"
#include "core/exception/exception.h"
#include "core/memory/ref.h"

#include <flatbuffers/flatbuffers.h>

#include "guess_machine.h"
#include "property_type.h"

namespace flatCrawler
{
	class TableRoot;
	class Sequence;
	class Table;
	class Struct;

	class Property
	{
	public:
		// Flatbuffer (V)Table
		using FBTable = flatbuffers::Table;

		// Flatbuffer "Dynamic" Vector
		using FBDVector = flatbuffers::Vector<flatbuffers::Offset<void>>;
	public:
		Property(TableRoot& root);
		~Property();

		wk::Ref<Property> from_fbtype(Property&);

	public:
		void from_pointer(const void* data);

	public:
		const FBTable* as_fbtable(bool is_root = false) const;
		const FBDVector* as_fbvector() const;
		bool is_valid_fbtable() const;
		bool is_dynamic_fbvector() const;
		bool is_valid_fbvector();
		bool is_valid_fbstring() const;
		bool is_valid_flexbuffer();

		template<typename T>
		bool is_valid_pointer(const T* ptr, bool aligned = true) const
		{
			const uint8_t* buffer = (uint8_t*)root.data;

			const auto offset = static_cast<size_t>((uint8_t*)ptr - buffer);
			
			if (!root.verifier->Verify(offset, 1)) return false;
			if (aligned)
			{
				bool align = root.verifier->VerifyAlignment(offset, sizeof(flatbuffers::soffset_t));
				if (!align) return false;
			}

			return true;
		}

		void guess_type();

	public:
		virtual bool is_property_type() const;
		virtual bool is_sequence_type() const;
		virtual bool is_vector_type() const;

		virtual bool is_sequence() const { return false; }

		virtual void write_schema(SchemaWriter& /*writer*/) { };
		virtual bool read() { return true; };

	public:
		Sequence& as_sequence() const;

	public:
		PropType::Type get_basetype() const;
		std::string get_basetype_name() const;

		PropType::VectorType vector_type = PropType::VectorType::Invalid;

	public:
		size_t field_size = 0;
		flatbuffers::voffset_t field_offset = 0;
		const void* data = nullptr;
		
		TableRoot& root;

	private:
		GuessMachine<PropType::Type> m_type;
	};
}