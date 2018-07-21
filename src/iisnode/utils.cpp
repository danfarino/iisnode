#include "precomp.h"
#include <stack>

class FindFileHandle {
public:
	FindFileHandle(HANDLE h) : handle(h) {}
	~FindFileHandle() {
		if (valid()) {
			FindClose(handle);
		}
	}

	operator HANDLE() { return handle; }
	operator bool() { return valid(); }

	bool valid() {
		return handle != INVALID_HANDLE_VALUE;
	}

private:
	HANDLE handle;
};

void FindFileRecursive(const std::wstring &searchSpec, const std::function<bool(const WIN32_FIND_DATAW &findData)> callback) {
	WIN32_FIND_DATAW findData = { 0 };

	const auto lastDelimIndex = searchSpec.rfind(L'\\');
	const auto root = searchSpec.substr(0, lastDelimIndex);
	const auto wildcard = searchSpec.substr(lastDelimIndex + 1);

	std::stack<std::wstring> directories({ root });

	while (!directories.empty()) {
		const auto directory = directories.top();
		directories.pop();

		// Find files
		{
			const auto directoryWithWildcard = directory + L'\\' + wildcard;
			FindFileHandle handle = FindFirstFileW(directoryWithWildcard.c_str(), &findData);

			if (handle) {
				do {
					if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
						auto keepGoing = callback(findData);
						if (!keepGoing) {
							break;
						}
					}
				} while (FindNextFileW(handle, &findData));
			}
		}

		// Find subdirectories
		{
			const auto directoryWithWildcard = directory + L"\\*";
			FindFileHandle handle = FindFirstFileW(directoryWithWildcard.c_str(), &findData);

			if (handle) {
				do {
					if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
						std::wstring filename(findData.cFileName);
						if (filename != L"." && filename != L"..") {
							directories.emplace(directory + L'\\' + filename);
						}
					}
				} while (FindNextFileW(handle, &findData));
			}
		}
	}
}
