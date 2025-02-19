#pragma once

#include "crawler/property.h"

namespace flatCrawler
{
	class Sequence : public Property
	{
		friend Table;

	public:
		using Property::Property;
		Sequence(Property& prop);

	public:
		virtual bool is_sequence() const { return true; }
		virtual bool is_table() const { return false; }
		virtual bool is_struct() const { return false; }
		virtual bool read();

	public:
		void set_sequence_id(uint32_t id);

	protected:
		std::string m_seq_name = "";
		uint32_t m_seq_id = 0;
	};
}