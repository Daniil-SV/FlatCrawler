#pragma once

#include "types/table.h"

namespace flatCrawler
{
	class TableRoot : public Table
	{
		friend Table;
	public:
		TableRoot();

	public:
		void from_file(const std::filesystem::path& path);
		virtual bool read() override;
		virtual void write_schema(SchemaWriter& writer) override;

	protected:
		void set_size_prefix();

	public:
		bool has_identifier = false;
		std::array<char, flatbuffers::kFileIdentifierLength> identifier = { '\0', '\0', '\0', '\0' };
		std::vector<uint8_t> buffer;

	protected:
		uint32_t table_counter = 0;
	};
}