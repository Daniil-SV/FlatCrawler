#include "root.h"

#include "core/io/file_stream.h"

namespace flatCrawler
{
	TableRoot::TableRoot() : Table(*this)
	{
	}

	void TableRoot::from_file(const std::filesystem::path& path, int offset)
	{
		wk::InputFileStream file(path);
		if (!file.is_open()) throw wk::Exception("Failed to open file %s", path.string().c_str());

		size_t length = file.length() - offset;
		buffer.resize(length);

		file.seek(offset);
		file.read(buffer.data(), length);

		data = buffer.data();
		verifier = wk::CreateRef<flatbuffers::SizeVerifier>((const uint8_t*)data, buffer.size());
	}

	void TableRoot::set_size_prefix()
	{
		uint32_t maybe_size = flatbuffers::ReadScalar<uint32_t>(data);
		size_prefixed = root.buffer.size() == maybe_size + sizeof(uint32_t);

		if (size_prefixed)
		{
			data = (uint8_t*)data + sizeof(uint32_t);
		}
	}

	bool TableRoot::read()
	{
		set_size_prefix();
		if (!is_valid_fbtable()) return false;

		bool success = Table::read();
		if (success)
		{
			// virtual table should start right after offset to it, 
			// but sometimes this may not be the case, and this means that most likely there is some data between them, like an identifier
			uint8_t* vtable_begin = (uint8_t*)data + sizeof(uint32_t);
			ptrdiff_t distance = vtable - vtable_begin;

			if (distance >= 4)
			{
				// copy identifier chars to array
				has_identifier = true;
				wk::Memory::copy(vtable_begin, identifier.data(), identifier.size());
			}
		}

		return success;
	}

	void TableRoot::write_schema(SchemaWriter& writer)
	{
		Table::write_schema(writer);
		writer.set_root_table(m_seq_name);

		if (has_identifier)
		{
			writer.set_identifier(std::string(identifier.begin(), identifier.end()));
		}
	}
}