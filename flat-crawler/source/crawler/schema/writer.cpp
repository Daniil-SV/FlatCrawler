#include "writer.h"

namespace flatCrawler
{
	SchemaWriter::SchemaWriter(const std::filesystem::path& path)
	{
		m_file.open(path);
		if (m_file.bad())
		{
			throw wk::Exception("Failed to open file %s for writing", path.string().c_str());
		}
	}

	const std::string SchemaWriter::GetTypename(flatbuffers::ElementaryType type)
	{
		switch (type)
		{
		case flatbuffers::ET_BOOL:
			return "bool";
		case flatbuffers::ET_CHAR:
			return "byte";
		case flatbuffers::ET_UCHAR:
			return "ubyte";
		case flatbuffers::ET_SHORT:
			return "short";
		case flatbuffers::ET_USHORT:
			return "ushort";
		case flatbuffers::ET_INT:
			return "int";
		case flatbuffers::ET_UINT:
			return "uint";
		case flatbuffers::ET_LONG:
			return "long";
		case flatbuffers::ET_ULONG:
			return "ulong";
		case flatbuffers::ET_FLOAT:
			return "float";
		case flatbuffers::ET_DOUBLE:
			return "double";
		case flatbuffers::ET_STRING:
			return "string";
		default:
			return "";
		}
	}

	void SchemaWriter::begin_table_seq(const std::string& name)
	{
		m_file << "table " << name << " {" << std::endl;
	}

	void SchemaWriter::begin_struct_seq(const std::string& name)
	{
		m_file << "struct " << name << " {" << std::endl;
	}

	void SchemaWriter::end_seq()
	{
		m_file << "}" << std::endl << std::endl;
	}

	void SchemaWriter::write_property(const std::string& name, const std::string& type)
	{
		m_file << "  " << name << ": " << type << ";" << std::endl;
	}

	void SchemaWriter::write_vector_property(const std::string& name, const std::string& type, size_t count)
	{
		m_file << "  " << name << ": [" << type;
		if (count)
		{
			m_file << ":" << count;
		}

		m_file << "];" << std::endl;
	}

	void SchemaWriter::set_root_table(const std::string& name)
	{
		m_file << "root_type " << name << ";" << std::endl;
	}

	void SchemaWriter::set_identifier(const std::string& name)
	{
		m_file << "file_identifier \"" << name << "\";" << std::endl;
	}
}