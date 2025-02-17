
#include "core/console/console.h"

#include "crawler/root.h"

using namespace flatCrawler;

int main(int argc, char* argv[])
{
	wk::ArgumentParser program("FlatCrawler");

	program
		.add_argument("input")
		.append()
		.help("Input flatbuffer files to analyze");

	program
		.add_argument("output")
		.append()
		.help("Output path for .fbs file");

	try
	{
		program.parse_args(argc, argv);
	}
	catch (const std::exception& e)
	{
		std::cout << program << std::endl;
		std::cout << "Error >>> " << e.what();
	}

	auto files = program.get<std::vector<std::string>>("input");
	auto output = program.get<std::string>("output");

	TableRoot current_table;
	for (auto& path : files)
	{
		//TableRoot current_table;
		current_table.from_file(path);
		bool success = current_table.read();

		if (!success)
		{
			std::cout << "The file is corrupted or is not flatbuffer file. Skipping " << path << "..." << std::endl;
			continue;
		}
	}

	SchemaWriter writer(output);
	current_table.write_schema(writer);
}