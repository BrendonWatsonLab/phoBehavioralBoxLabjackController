#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <experimental/filesystem>
#include <regex>
#include <type_traits>
#include <map>
#include "LabjackDataFile.h"

// If Windows (32 or 64 bit)
//#ifdef _WIN32
//#include <string>
//#include <windows.h>
//#else
//#include <string>
//#include <limits.h>
//#include <unistd.h>
//#endif

namespace fs = std::experimental::filesystem;

// literal '.' followed by one of "cpp", "cc", "cxx", "h", "hh", "hpp" or "hxx"
// note: ?: indicates that it is a non-capturing group
static const std::regex csv_files("\\.(?:csv|CSV)");

class FilesystemHelpers
{
public:
	FilesystemHelpers() {};
	//~FilesystemHelpers() {};

	// list of paths of all files under the directory 'dir' when the extenstion matches the regex
	// file_list<true> searches recursively into sub-directories; file_list<false> searches only the specified directory
	template < bool RECURSIVE > static std::vector<fs::path> file_list(fs::path dir, std::regex ext_pattern);

	// non-recursive scan for c++ files: if dir is omitted, current directory is scanned 
	std::vector<fs::path> static scan_csv_files(fs::path dir = ".");

	// recursive scan for c++ files: if dir is omitted, current directory is scanned 
	std::vector<fs::path> static rscan_csv_files(fs::path dir = ".");

	// out_file_s470019538_1566513006823
	// Finds all the Labjack data files in the searchDirectory and returns a map indexed by the labjack serial number
	static std::map<int, std::vector<LabjackDataFile>> findDataFiles(std::string searchDirectory);

	////TODO: concatenate CSV Files needs to be implemented.
	//static void concatenateCsvFiles(std::vector<LabjackDataFile> dataFiles_);

	//static std::string getExePath();

	static bool createDirectory(std::string path);

};


