#pragma once

#include "sequence/table.h"
#include "sequence/struct.h"
#include "sequence/vector.h"

namespace flatCrawler
{
	class TableRoot : public Table
	{
		friend Table;
		friend Sequence;
	public:
		TableRoot();

	public:
		void from_file(const std::filesystem::path& path, int offset = 0);
		virtual bool read() override;
		virtual void write_schema(SchemaWriter& writer) override;

	protected:
		void set_size_prefix();

	public:
		bool size_prefixed = false;
		bool has_identifier = false;
		std::array<char, flatbuffers::kFileIdentifierLength> identifier = { '\0', '\0', '\0', '\0' };
		std::vector<uint8_t> buffer;
		wk::Ref<flatbuffers::SizeVerifier> verifier;

	protected:
		uint32_t sequence_counter = 0;
	};
}