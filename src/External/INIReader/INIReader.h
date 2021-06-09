// Read an INI file into easy-to-access name/value pairs.

// SPDX-License-Identifier: BSD-3-Clause

// Copyright (C) 2009-2020, Ben Hoyt

// inih and INIReader are released under the New BSD license (see LICENSE.txt).
// Go to the project home page for more info:
//
// https://github.com/benhoyt/inih

#ifndef INIREADER_H
#define INIREADER_H

#include <map>
#include <string>
#include <vector>

// Read an INI file into easy-to-access name/value pairs. (Note that I've gone
// for simplicity here rather than speed, but it should be pretty decent.)
class INIReader
{
public:
    // Construct INIReader and parse given filename. See ini.h for more info
    // about the parsing.
    explicit INIReader(const std::string& filename);

    // Construct INIReader and parse given buffer. See ini.h for more info
    // about the parsing.
    explicit INIReader(const char *buffer, size_t buffer_size);

    // Return the result of ini_parse(), i.e., 0 on success, line number of
    // first error on parse error, or -1 on file open error.
    int ParseError() const;

    // Get a string value from INI file, returning default_value if not found.
    std::string Get(const std::string& section, const std::string& name, const std::string& default_value);

    // Get a string value from INI file, returning default_value if not found,
    // empty, or contains only whitespace.
    std::string GetString(const std::string& section, const std::string& name, const std::string& default_value);

    // Get an integer (long) value from INI file, returning default_value if
    // not found or not a valid integer (decimal "1234", "-1234", or hex "0x4d2").
    long GetInteger(const std::string& section, const std::string& name, long default_value);

    // Get a real (floating point double) value from INI file, returning
    // default_value if not found or not a valid floating point value
    // according to strtod().
    double GetReal(const std::string& section, const std::string& name, double default_value);

    // Get a boolean value from INI file, returning default_value if not found or if
    // not a valid true/false value. Valid true values are "true", "yes", "on", "1",
    // and valid false values are "false", "no", "off", "0" (not case sensitive).
    bool GetBoolean(const std::string& section, const std::string& name, bool default_value);

    // Return true if the given section exists (section must contain at least
    // one name=value pair).
    bool HasSection(const std::string& section) const;

    // Return true if a value exists with the given section and field names.
    bool HasValue(const std::string& section, const std::string& name) const;

	// Dynamic Building Helpers
    void beginDynamicIniBuilding();
    void endDynamicIniBuilding();

    bool writeDynamicIni(std::string path);

private:

    static bool ToBoolean(std::string valstr, bool default_value);
    static long ToInteger(std::string valstr, long default_value);
    static double ToReal(std::string valstr, double default_value);

    static std::string FromBoolean(bool final_value);
    static std::string FromInteger(long final_value);
    static std::string FromReal(double final_value);
	
    bool enableDynamicIniBuilding = false;
    //std::map<std::string, std::vector<std::string>> _dynamicIniBuilder;
    std::map<std::string, std::vector<std::pair<std::string, std::string>>> _dynamicIniBuilder;
	
    void addDynamic(std::string section, std::string name, std::string value);

	
    int _error;
    std::map<std::string, std::string> _values;
    static std::string MakeKey(const std::string& section, const std::string& name);
    static int ValueHandler(void* user, const char* section, const char* name, const char* value);
};

#endif  // INIREADER_H
