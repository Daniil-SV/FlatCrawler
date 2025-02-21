#include "sequence.h"
#include "crawler/root.h"

#include "crawler/sequence/vector.h"

namespace flatCrawler
{
	Sequence::Sequence(Property& prop) : Property(prop)
	{
	}

	Vector& Sequence::as_vector() const
	{
		if (!is_vector_type()) { assert(0); }

		return *(Vector*)this;
	}

	Table& Sequence::as_table() const
	{
		if (!is_table()) { assert(0); }

		return *(Table*)this;
	}

	bool Sequence::read()
	{
		set_sequence_id(root.sequence_counter++);
		return true;
	}

	void Sequence::set_sequence_id(uint32_t id)
	{
		m_seq_id = id;
		std::string basename;
		if (is_struct())
		{
			basename = "Struct";
		}
		else
		{
			basename = "Table";
		}

		m_seq_name = basename + std::to_string(id);
	}
}