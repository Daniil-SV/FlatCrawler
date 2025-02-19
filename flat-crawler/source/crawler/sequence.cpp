#include "sequence.h"
#include "crawler/root.h"

namespace flatCrawler
{
	Sequence::Sequence(Property& prop) : Property(prop)
	{
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