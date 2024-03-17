#ifndef IMPATH_H
#define IMPATH_H

#include <iostream>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace IME
{
	/*!
	 * \brief Gets the path to the executable.
	 * 
	 * \return Path to executable
	 */
	std::string GetExecutablePath()
	{
		char path[2048];

#ifdef _WIN32
		GetModuleFileName(NULL, path, sizeof(path));
#else
		ssize_t count = readlink("/proc/self/exe", path, sizeof(path));
		if (count == -1) return "";
		path[count] = '\0';
#endif
		std::string pathStr = std::string(path);
		return pathStr.substr(0, pathStr.find_last_of("\\/"));
	}
}

#endif // IMPATH_H