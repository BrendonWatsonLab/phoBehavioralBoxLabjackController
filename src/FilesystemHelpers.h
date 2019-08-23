#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <experimental/filesystem>
#include <regex>
#include <type_traits>

namespace fs = std::experimental::filesystem;

// list of paths of all files under the directory 'dir' when the extenstion matches the regex
// file_list<true> searches recursively into sub-directories; file_list<false> searches only the specified directory
template < bool RECURSIVE > std::vector<fs::path> file_list(fs::path dir, std::regex ext_pattern)
{
	std::vector<fs::path> result;

	using iterator = std::conditional< RECURSIVE,
		fs::recursive_directory_iterator, fs::directory_iterator >::type;

	const iterator end;
	for (iterator iter{ dir }; iter != end; ++iter)
	{
		const std::string extension = iter->path().extension().string();
		if (fs::is_regular_file(*iter) && std::regex_match(extension, ext_pattern)) result.push_back(*iter);
	}

	return result;
}

//// literal '.' followed by one of "cpp", "cc", "cxx", "h", "hh", "hpp" or "hxx"
//// note: ?: indicates that it is a non-capturing group
//static const std::regex cpp_files("\\.(?:cpp|cc|cxx|h|hh|hpp|hxx)");
//
//// non-recursive scan for c++ files: if dir is omitted, current directory is scanned 
//std::vector<fs::path> scan_cpp_files(fs::path dir = ".") { return file_list<false>(dir, cpp_files); }
//
//// recursive scan for c++ files: if dir is omitted, current directory is scanned 
//std::vector<fs::path> rscan_cpp_files(fs::path dir = ".") { return file_list<true>(dir, cpp_files); }


// literal '.' followed by one of "cpp", "cc", "cxx", "h", "hh", "hpp" or "hxx"
// note: ?: indicates that it is a non-capturing group
static const std::regex csv_files("\\.(?:csv|CSV)");

// non-recursive scan for c++ files: if dir is omitted, current directory is scanned 
std::vector<fs::path> scan_csv_files(fs::path dir = ".") { return file_list<false>(dir, csv_files); }

// recursive scan for c++ files: if dir is omitted, current directory is scanned 
std::vector<fs::path> rscan_csv_files(fs::path dir = ".") { return file_list<true>(dir, csv_files); }


// out_file_s470019538_1566513006823

