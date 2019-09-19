/*
 ********************************
 *  Created on: Sep 19, 2019
 *      Author: Andriy Andriychuk
 ********************************
 */

#include <iostream>
#include <vector>
#include <string>
#include <windows.h>

using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::string;

/*
 * @brief Represents values of specific type, that says what part of filename
 * should be compared with the given pattern.
 *
 * For instance, if pattern is equal to "*.xxx", flag is set to "EXTENSION" and
 * then an extension of file is compared with extension of pattern.
 * The value "NOTHING" means, that comparison must be skipped (find all files).
 */
enum class CMP_FILENAME_PART
{
	NOTHING,
	FILE_NAME,
	EXTENSION,
	EVERYTHING
};

/*
 * @brief Goes through directories recursively and gets filenames that match 
 * given pattern name.
 *
 * @param[in]	dirPath					: path to a directory, where files search is 
 *														  started from.
 * @param[out] files				  : vector of found files during the search.
 * @param[in] fileNamePattern : filenames pattern by which files are going to 
 *															be searched in directory.
 * @param[in] cmpFileNamePart	: variable, that specify what part of filename
 *															should be compared to match the given pattern.
 */
void getFiles(const std::string& dirPath, 
							vector<string>& files, 
							const string& fileNamePattern,
							const CMP_FILENAME_PART& cmpFileNamePart);

/*
 * @brief Gets cmpFileNamePart variable based on the given filename pattern.
 *
 * @param[in] fileNamePattern : the given filename pattern.
 *
 * @return cmpFileNamePart variable, that says what part of filename should be 
 * compared with pattern filename.
 */
const CMP_FILENAME_PART getCmpFileNamePart(const string& fileNamePattern);

int main(int argc, char **argv)
{
	string dirPath, fileNamePattern;
	if (argc == 3)
	{
		dirPath = string(argv[1]);
		fileNamePattern = string(argv[2]);
	}
	else // some arguments are missed
	{
		// The directory path must be entered without spaces
		cout << "Enter a directory path: " << endl;
		cin >> dirPath;
		cout << "Enter a pattern for filename: " << endl;
		cin >> fileNamePattern;
	}
	cout << "Searching of files by \"" << fileNamePattern << "\" pattern name "
					"in \"" << dirPath << "\" directory." << endl;
	vector<string> files;
	const CMP_FILENAME_PART cmpFileNamePart = getCmpFileNamePart(fileNamePattern);

	getFiles(dirPath, files, fileNamePattern, cmpFileNamePart);
	
	if (!files.empty())
	{
		cout << "Files that match a pattern:" << endl;
		for (auto file : files)
			cout << file << endl;
	}
	else
	{
		cout << "No files are found." << endl;
	}

	system("pause");
	return 0;
}

const CMP_FILENAME_PART getCmpFileNamePart(const string& fileNamePattern)
{
	CMP_FILENAME_PART cmpFileNamePart;
	if (fileNamePattern[0] == '*' && fileNamePattern[1] == '.')
	{
		if (fileNamePattern[2] == '*')
		{
			cmpFileNamePart = CMP_FILENAME_PART::NOTHING;
		}
		else
		{
			cmpFileNamePart = CMP_FILENAME_PART::EXTENSION;
		}
	}
	else if (*(fileNamePattern.end() - 2) == '.' && 
					 *(fileNamePattern.end()-1) == '*')
	{
		cmpFileNamePart = CMP_FILENAME_PART::FILE_NAME;
	}
	else
	{
		cmpFileNamePart = CMP_FILENAME_PART::EVERYTHING;
	}
	return cmpFileNamePart;
}

void getFiles(const std::string& dirPath, 
							vector<string>& files, 
							const string& fileNamePattern, 
							const CMP_FILENAME_PART& cmpFileNamePart)
{
	string path(dirPath + "\\*");

	WIN32_FIND_DATA data;
	HANDLE hFind = FindFirstFile(path.c_str(), &data);
	if (hFind == INVALID_HANDLE_VALUE) {
		if (GetLastError() != ERROR_FILE_NOT_FOUND)
			// Error code examples:
			// 5 - ERROR_ACCESS_DENIED
			// 123 - ERROR_INVALID_NAME (e.g. if folder name is in cyrillic)
			cout << "The FindFirstFile() function failed with error code: " 
			     << GetLastError() << "." << endl;

		// otherwise the folder is empty (nothing to do)
		return;
	}

	do {
		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (data.cFileName == string(".") || data.cFileName == string(".."))
				continue;

			string newPath(dirPath + "\\" + data.cFileName);
			getFiles(newPath, files, fileNamePattern, cmpFileNamePart);
		}
		else
		{
			string fileName(data.cFileName);
			switch (cmpFileNamePart)
			{
			case CMP_FILENAME_PART::NOTHING:
				files.push_back(dirPath + "\\" + fileName);
				break;

			case CMP_FILENAME_PART::EVERYTHING:
				if (fileName == fileNamePattern)
					files.push_back(dirPath + "\\" + fileName);
				break;

			case CMP_FILENAME_PART::EXTENSION:
				if (fileName.substr(fileName.find('.') + 1, 
														fileName.size() - fileName.find('.')) ==
					  fileNamePattern.substr(2, fileNamePattern.size() - 
																		  fileNamePattern.find('.')))
				{
					files.push_back(dirPath + "\\" + fileName);
				}
				break;

			case CMP_FILENAME_PART::FILE_NAME:
				if (fileName.substr(0, fileName.find('.')) ==
					  fileNamePattern.substr(0, fileNamePattern.find('.')))
				{
					files.push_back(dirPath + "\\" + fileName);
				}
				break;
			}
		}
	} while (FindNextFile(hFind, &data) != 0);
	FindClose(hFind);
}