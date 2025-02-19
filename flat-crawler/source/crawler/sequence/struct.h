#pragma once

#include "crawler/sequence.h"

namespace flatCrawler
{
	class Struct : public Sequence
	{
	public:
		using Sequence::Sequence;

	public:
		virtual bool is_struct() const { return true; }

	public:
		virtual void write_schema(SchemaWriter& writer) override;
	};
}