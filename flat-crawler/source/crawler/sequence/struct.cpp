#include "struct.h"

namespace flatCrawler
{
	void Struct::write_schema(SchemaWriter& writer)
	{
		writer.begin_struct_seq(m_seq_name);

		writer.write_vector_property(
			"unk_data",
			SchemaWriter::GetTypename(flatbuffers::ElementaryType::ET_UCHAR),
			field_size
		);

		writer.end_seq();
	}
}

