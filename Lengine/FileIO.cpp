#include "FileIO.h"
#include<fstream>
#include<iostream>
namespace Lengine {
	bool FileIO::fileIntoBuffer(const std::string& filePath, std::vector<unsigned char>& buffer) {

		std::ifstream Filein(filePath, std::ios::binary);

		//open file error checking
		if (Filein.fail())
		{
			perror(filePath.c_str());
			return false;
		}

		//seek to the end of the file
		Filein.seekg(0, std::ios::end);
		//x byte offset(1) to the position(2)
		//take the file pointer(used to read file) to the end

		//get the file size
		int Filesize = Filein.tellg();
		//restore how many char has been passed

		//get back to the beginning and begin to read the file
		Filein.seekg(0, std::ios::beg);

		//resize the buffer to prepare
		buffer.resize(Filesize);

		//read file to the buffer
		Filein.read((char*)&buffer[0], Filesize);
		//the beginning of the contaner, read size

		//close the file
		Filein.close();

		//no fault return true
		return true;

	}
	bool FileIO::fileIntoBuffer(const std::string& filePath, std::string& buffer) {

		std::ifstream Filein(filePath);

		//open file error checking
		if (Filein.fail())
		{
			perror(filePath.c_str());
			return false;
		}

		//seek to the end of the file
		Filein.seekg(0, std::ios::end);
		//x byte offset(1) to the position(2)
		//take the file pointer(used to read file) to the end

		//get the file size
		int Filesize = Filein.tellg();
		//store how many char has been passed

		//get back to the beginning and begin to read the file
		Filein.seekg(0, std::ios::beg);

		//resize the buffer to prepare
		buffer.resize(Filesize);

		//read file to the buffer
		Filein.read((char*)&buffer[0], Filesize);
		//the beginning of the contaner, read size

		//close the file
		Filein.close();

		//no fault return true
		return true;

	}
	bool FileIO::getDirectoryEntries(const char* filePath, std::vector<DivEntry>& entriesContainer) {
		fs::path Path(filePath);
		entriesContainer.clear();
		if (!fs::is_directory(Path)) return false;
		for (auto It = fs::directory_iterator(Path); It != fs::directory_iterator(); It++) {
			entriesContainer.emplace_back();
			entriesContainer.back().path = It->path().string();
			entriesContainer.back().isDirectory = fs::is_directory(It->path());
		}
		return true;
	}
	bool FileIO::createDirectory(const char* filePath) {
		return fs::create_directory(fs::path(filePath));
	}
}