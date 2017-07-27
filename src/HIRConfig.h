#pragma once

#include <string>
#include <sstream>
#include <ios> // boolalpha
#include <vector>
#include <unordered_map>
#include <utility> // pair

class HIRConfig {
public:
	//file name
	std::string mfname = "model.txt";
	std::string tfname = "train.txt";
	std::string efname = "test.txt";
	// Data specification
	int rtrain = 0;
	int rtest = 0;
	int fwid = 0;
	int task = 1;//regression default
	bool ismat = false;//tensor default
	int e_num = 0;
	int hnum = 1;
	int dim = 10;
	// Size of parameter
	std::vector<int> maxe, idxe, size;
	
	HIRConfig();
	~HIRConfig();
};

class HIRParser
{
public:
	HIRParser(const std::string& filename);
	~HIRParser();
	//int HIRParsering(HIRConfig cfg);

	template <typename T, typename... Args>
	void get(const std::string& key, T& value, Args&... args) const;
	template <typename T>
	void get(const std::string& key, std::vector<T>& vector) const;

	template <typename T, typename... Args>
	void set(const std::string& key, const T& value, const Args&... args);
	template <typename T>
	void set(const std::string& key, const std::vector<T>& vector);

private:
	typedef std::pair<std::string, std::string> KeyValuePair;
	template <typename T>
	void toValue(std::string& str, T& value) const;
	template <typename T>
	void toValue(std::istringstream& iss, T& value) const;

	void trim(std::string& string) const; // trim whitespace from string
	KeyValuePair parse(const std::string& line) const;

private:
	bool modified = false;
	std::string filename;
	std::unordered_map<std::string, std::string> configMap;
};
template <typename T>
void HIRParser::toValue(std::string& str, T& value) const
{
	std::istringstream iss(str);
	iss >> std::boolalpha >> value;
}

template <>
inline void HIRParser::toValue<std::string>(std::string& str, std::string& value) const
{
	trim(str);
	value = str;
}

template <typename T>
void HIRParser::toValue(std::istringstream& iss, T& value) const
{
	std::string str;
	if (std::getline(iss, str, ','))
		toValue(str, value);
}

template <typename T, typename... Args>
void HIRParser::get(const std::string& key, T& value, Args&... args) const
{
	auto found = configMap.find(key);
	if (found != configMap.end())
	{
		std::istringstream iss(found->second);
		toValue(iss, value);
		int dummy[] = { 0, (toValue(iss, args), 0) ... }; // parameter pack expansion
	}
}

template <>
inline void HIRParser::get<std::string>(const std::string& key, std::string& value) const
{
	auto found = configMap.find(key);
	if (found != configMap.end())
		value = found->second;
}

template <typename T>
void HIRParser::get(const std::string& key, std::vector<T>& vector) const
{
	auto found = configMap.find(key);
	if (found != configMap.end())
	{
		std::istringstream iss(found->second);
		std::string str;
		while (std::getline(iss, str, ','))
		{
			vector.emplace_back();
			std::istringstream(str) >> std::boolalpha >> vector.back();
		}
	}
}

template <typename T, typename... Args>
void HIRParser::set(const std::string& key, const T& value, const Args&... args)
{
	std::ostringstream oss;
	oss << std::boolalpha << value;
	int dummy[] = { 0, (oss << ", " << args, 0) ... }; // parameter pack expansion

	if (configMap[key] != oss.str())
	{
		configMap[key] = oss.str();
		modified = true;
	}
}

template <typename T>
void HIRParser::set(const std::string& key, const std::vector<T>& vector)
{
	std::ostringstream oss;
	oss << std::boolalpha;
	for (auto it = vector.begin(); it != vector.end(); ++it)
	{
		if (it != vector.begin())
			oss << ", ";
		oss << *it;
	}

	if (configMap[key] != oss.str())
	{
		configMap[key] = oss.str();
		modified = true;
	}
}
