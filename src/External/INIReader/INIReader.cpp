// Read an INI file into easy-to-access name/value pairs.

// SPDX-License-Identifier: BSD-3-Clause

// Copyright (C) 2009-2020, Ben Hoyt

// inih and INIReader are released under the New BSD license (see LICENSE.txt).
// Go to the project home page for more info:
//
// https://github.com/benhoyt/inih

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "ini.h"
#include "INIReader.h"

using std::string;

INIReader::INIReader(const string& filename)
{
    _error = ini_parse(filename.c_str(), ValueHandler, this);
}

INIReader::INIReader(const char *buffer, size_t buffer_size)
{
  string content(buffer, buffer_size);
  _error = ini_parse_string(content.c_str(), ValueHandler, this);
}

int INIReader::ParseError() const
{
    return _error;
}

string INIReader::Get(const string& section, const string& name, const string& default_value)
{
    string key = MakeKey(section, name);
	/*if (this->enableDynamicIniBuilding)
	{
		auto gotValue = _values.count(key) ? _values.find(key)->second : default_value;
        this->addDynamic(section, name, gotValue);
				
	}*/
    // Use _values.find() here instead of _values.at() to support pre C++11 compilers
    return _values.count(key) ? _values.find(key)->second : default_value;
}

string INIReader::GetString(const string& section, const string& name, const string& default_value)
{
    const string str = Get(section, name, "");
    auto final_result = str.empty() ? default_value : str;
    if (this->enableDynamicIniBuilding)
    {
        this->addDynamic(section, name, final_result);
    }
	return final_result;
}

long INIReader::GetInteger(const string& section, const string& name, long default_value)
{
    string valstr = Get(section, name, "");
    auto final_result = INIReader::ToInteger(valstr, default_value);
    if (this->enableDynamicIniBuilding)
    {
        this->addDynamic(section, name, INIReader::FromInteger(final_result));
    }
    return final_result;
}

double INIReader::GetReal(const string& section, const string& name, double default_value)
{
    string valstr = Get(section, name, "");
    auto final_result = INIReader::ToReal(valstr, default_value);
    if (this->enableDynamicIniBuilding)
    {
        this->addDynamic(section, name, INIReader::FromReal(final_result));
    }
    return final_result;
}

bool INIReader::GetBoolean(const string& section, const string& name, bool default_value)
{
    string valstr = Get(section, name, "");
	auto final_result = INIReader::ToBoolean(valstr, default_value);
    if (this->enableDynamicIniBuilding)
    {
        this->addDynamic(section, name, INIReader::FromBoolean(final_result));
    }
    return final_result;
}

bool INIReader::HasSection(const string& section) const
{
    const string key = MakeKey(section, "");
    std::map<string, string>::const_iterator pos = _values.lower_bound(key);
    if (pos == _values.end())
        return false;
    // Does the key at the lower_bound pos start with "section"?
    return pos->first.compare(0, key.length(), key) == 0;
}

bool INIReader::HasValue(const string& section, const string& name) const
{
    string key = MakeKey(section, name);
    return _values.count(key);
}

void INIReader::beginDynamicIniBuilding()
{
    this->_dynamicIniBuilder.clear(); // Clear existing dynamic ini building session
    this->enableDynamicIniBuilding = true;
    return;
}

void INIReader::endDynamicIniBuilding()
{
    this->_dynamicIniBuilder.clear(); // Clears the accumulated dynamic ini building session
    this->enableDynamicIniBuilding = false;
}

bool INIReader::writeDynamicIni(std::string path)
{
    if (!this->enableDynamicIniBuilding)
    {
        return false;
    }
	if (this->_dynamicIniBuilder.empty())
	{
        return false;
	}
    std::ofstream myNewINIFile;
    myNewINIFile.open(path);
    for (const auto& [key, sectionItems] : this->_dynamicIniBuilder) {
        //itemKeys.push_back(key);
        // Write out the section:
        myNewINIFile << "[" << key << "]\n";
    	// Iterate through the keys:
        for (const auto& [aSectionItemKey, aSectionItemValue] : sectionItems)
        {
            myNewINIFile << aSectionItemKey << "=" << aSectionItemValue << "\n";
        }
        myNewINIFile << " \n"; // Blank line at the end of the section
    }
    myNewINIFile.close(); // Close the file

    this->endDynamicIniBuilding(); // Signal that we're done with dynamic ini building
    return true;
}

bool INIReader::ToBoolean(std::string valstr, bool default_value)
{
    // Convert to lower case to make string comparisons case-insensitive
    std::transform(valstr.begin(), valstr.end(), valstr.begin(), ::tolower);
    if (valstr == "true" || valstr == "yes" || valstr == "on" || valstr == "1")
        return true;
    else if (valstr == "false" || valstr == "no" || valstr == "off" || valstr == "0")
        return false;
    else
        return default_value;
}

long INIReader::ToInteger(std::string valstr, long default_value)
{
    const char* value = valstr.c_str();
    char* end;
    // This parses "1234" (decimal) and also "0x4D2" (hex)
    long n = strtol(value, &end, 0);
    return end > value ? n : default_value;
}

double INIReader::ToReal(std::string valstr, double default_value)
{
    const char* value = valstr.c_str();
    char* end;
    double n = strtod(value, &end);
    return end > value ? n : default_value;
}

std::string INIReader::FromBoolean(bool final_value)
{
	if (final_value) {
        return "true";
	}
    else
    {
        return "false";
    }
}

std::string INIReader::FromInteger(long final_value)
{
    return std::to_string(final_value);
}

std::string INIReader::FromReal(double final_value)
{
    return std::to_string(final_value);
}

void INIReader::addDynamic(std::string section, std::string name, std::string value)
{
	if (!this->enableDynamicIniBuilding)
	{
        return;
	}

    auto finalValuePair = std::pair<std::string, std::string>(name, value);
	if (this->_dynamicIniBuilder.count(section))
	{
		// Section already exists
        this->_dynamicIniBuilder.at(section).push_back(finalValuePair);
	}
    else
    {
	    // Section needs to be created
        auto newVector = std::vector<std::pair<std::string, std::string>>();
        newVector.push_back(finalValuePair);
        //auto newVectorKeyValuePair = { section, newVector };
        /*this->_dynamicIniBuilder.insert(section, newVector);*/
        this->_dynamicIniBuilder.insert({ section, newVector });
    }
	
}

string INIReader::MakeKey(const string& section, const string& name)
{
    string key = section + "=" + name;
    // Convert to lower case to make section/name lookups case-insensitive
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    return key;
}

int INIReader::ValueHandler(void* user, const char* section, const char* name,
                            const char* value)
{
    if (!name)  // Happens when INI_CALL_HANDLER_ON_NEW_SECTION enabled
        return 1;
    INIReader* reader = static_cast<INIReader*>(user);
    string key = MakeKey(section, name);
    if (reader->_values[key].size() > 0)
        reader->_values[key] += "\n";
    reader->_values[key] += value ? value : "";
    return 1;
}
