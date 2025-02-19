#pragma once

#include "core/exception/exception.h"

#include <filesystem>

#include <iostream>
#include <fstream>

#include <string>

#include <flatbuffers/flatbuffers.h>

namespace flatCrawler
{
	class SchemaWriter
	{
	public:
		SchemaWriter(const std::filesystem::path& path);

		static const std::string GetTypename(flatbuffers::ElementaryType type);

	public:
		void begin_table_seq(const std::string& name);
		void begin_struct_seq(const std::string& name);
		void end_seq();

		void write_property(const std::string& name, const std::string& type);
		void write_vector_property(const std::string& name, const std::string& type, size_t count = 0);

		void set_root_table(const std::string& name);
		void set_identifier(const std::string& name);

	private:
		std::ofstream m_file;
	};
}