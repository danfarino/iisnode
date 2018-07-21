#pragma once

#include <string>
#include <sstream>
#include <memory>

class _makestr
{
public:
	_makestr() : temp_(new std::wostringstream)
	{

	}

	operator std::wstring() const
	{
		return temp_->str();
	}

	std::shared_ptr<std::wostringstream> temp_;
};

template <typename T>
const _makestr& operator<<(const _makestr &ms, const T &arg)
{
	*(ms.temp_) << arg;
	return ms;
}

inline const _makestr& operator<<(const _makestr &ms, std::basic_ostream<wchar_t, std::char_traits<wchar_t> >& (*fn)(std::basic_ostream<wchar_t, std::char_traits<wchar_t> >&))
{
	fn(*(ms.temp_));
	return ms;
}

#define MAKE_STRING(x) (static_cast<std::wstring>(_makestr() << x))
