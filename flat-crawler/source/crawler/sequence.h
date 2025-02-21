#pragma once

#include "crawler/property.h"

namespace flatCrawler
{
	class Vector;
	class Table;

	class Sequence : public Property
	{
		friend Table;

	public:
		using Property::Property;
		Sequence(Property& prop);

	public:
		Vector& as_vector() const;
		Table& as_table() const;

	public:
		virtual bool is_sequence() const { return true; }
		virtual bool is_table() const { return false; }
		virtual bool is_struct() const { return false; }
		virtual bool is_vector() const { return false; }
		virtual bool read();

	public:
		void set_sequence_id(uint32_t id);
		const std::string& sequence_name() const { return m_seq_name; };

	protected:
		std::string m_seq_name = "";
		uint32_t m_seq_id = 0;
	};
}