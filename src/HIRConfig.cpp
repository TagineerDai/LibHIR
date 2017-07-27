#include "HIRConfig.h"
#include <fstream>
#include <iostream>
#include <algorithm> // find_if
#include <cctype> // isalpha

HIRConfig::HIRConfig() {
	HIRParser parser = HIRParser("config.txt");
	parser.get("rtrain", rtrain);
	parser.get("rtest", rtest);
	
	parser.get("fwid", fwid);
	
	parser.get("task", task);
	
	ismat = false;
	parser.get("dim", dim);
	parser.get("hnum", hnum);
	parser.get("enum", e_num);

	parser.get("maxe", maxe);
	parser.get("idxe", idxe);
	parser.get("size", size);
}

HIRConfig::~HIRConfig() {
}

HIRParser::HIRParser(const std::string& filename)
	: filename(filename)
{
	std::ifstream ifs(filename);
	if (!ifs.is_open())
	{
		std::cout << "HIRParser::HIRParser - Unable to open " << filename << std::endl;
		return;
	}

	std::string line;
	while (std::getline(ifs, line))
	{
		trim(line);
		auto pair = parse(line);

		if (!pair.first.empty() && !pair.second.empty())
			configMap[pair.first] = pair.second; // overwrite if exists
	}

	ifs.close();
}

HIRParser::~HIRParser()
{
	if (!modified)
		return;

	std::vector<KeyValuePair> content;

	/* READ CONTENT */
	std::ifstream ifs(filename);
	if (ifs.is_open())
	{
		std::string line;
		while (std::getline(ifs, line))
		{
			trim(line);
			auto pair = parse(line);

			if (!pair.first.empty() && !pair.second.empty())
			{
				auto found = configMap.find(pair.first);
				if (found != configMap.end())
					pair.second = found->second;
			}

			else
			{
				pair.first = line;
				pair.second = "";
			}
			content.emplace_back(pair);
		}
		ifs.close();
	}

	// add elements that doesn't exist in a file but exist in configMap
	// opened to the user to modify the complexity of the network {useless
	for (auto it = configMap.begin(); it != configMap.end(); ++it)
	{
		auto found = std::find_if(content.begin(), content.end(), [&](const KeyValuePair& p)
		{
			return p.first == it->first;
		});

		if (found == content.end())
			content.emplace_back(it->first, it->second);
	}

	/* WRITE CONTENT */
	std::ofstream ofs(filename);
	if (!ofs.is_open())
	{
		std::cout << "HIRParser::~HIRParser - Unable to open " << filename << std::endl;
		return;
	}

	for (const auto& element : content)
	{
		ofs << element.first;
		if (!element.second.empty())
			ofs << " = " << element.second;
		ofs << std::endl;
	}
	ofs.close();
}

void HIRParser::trim(std::string& str) const
{
	std::size_t first = str.find_first_not_of(" \t\n");
	std::size_t last = str.find_last_not_of(" \t\n");

	if (first == std::string::npos) // empty string
		str = "";
	else
		str = str.substr(first, last - first + 1);
}

HIRParser::KeyValuePair HIRParser::parse(const std::string& line) const
{
	std::string key, value;
	std::size_t equal = line.find('=');

	if (equal != std::string::npos && std::isalpha(line.front())) // !line.empty()
	{
		key = line.substr(0, equal);
		value = line.substr(equal + 1);

		trim(key);
		trim(value);
	}

	return std::make_pair(key, value);
}
