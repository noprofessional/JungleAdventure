#pragma once
#include<vector>
#include<filesystem>
namespace fs = std::experimental::filesystem::v1;

namespace Lengine {
	struct DivEntry {
		std::string path;
		bool isDirectory;
	};
	class FileIO
	{
	public:
		static bool fileIntoBuffer(const std::string& filePath, std::vector<unsigned char>& buffer);
		static bool fileIntoBuffer(const std::string& filePath, std::string& buffer);
		//get all directories(include file) directly under filePath
		static bool getDirectoryEntries(const char* filePath, std::vector<DivEntry>& entriesContainer);
		//create a directory which has a relative path of filePath
		static bool createDirectory(const char* filePath);
	};
}
