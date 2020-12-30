// C string wrappers that provides constant-time erase front/back and substr operations
#pragma once

#include <string>
#include <cstring>

using std::string;

// This class doesn't copy string data on construction
// Will be invalidated as soon as the original string go out of scope
class tstring {
protected:
  const char* data;
  size_t pos, end_pos;

public:
  // const time operations
  tstring(const char*, size_t, size_t);
  tstring() : tstring(nullptr, 0, 0) {}
  tstring(const char* data, size_t length);
  tstring(const string&);
  explicit tstring(const tstring&);
  explicit tstring(const char*);

  size_t length() const;
  size_t size() const;
  tstring& erase_front(size_t = 1);
  tstring& erase_back(size_t = 1);
  tstring& set_length(size_t);
  tstring& ltrim(const char* trim_char = "\r\n\t\v\f ");
  tstring& rtrim(const char* trim_char = "\r\n\t\v\f ");
  tstring& trim(const char* trim_char = "\r\n\t\v\f ");
  tstring& trim_quotes();
  bool cut_front_back(const char* front, const char* back = "");
  bool empty() const;
  bool untouched() const;
  char front() const;
  char back() const;
  const char* begin() const;
  const char* end() const;
  tstring substr(size_t pos, size_t length) const;
  tstring substr(size_t pos) const;
  size_t get_end_pos() const;
  char operator[](size_t) const;

  // linear time operations
  template<typename T> std::strong_ordering compare(const T&) const;
  std::strong_ordering operator<=>(const tstring& other) const;
  std::strong_ordering operator<=>(const string& other) const;
  bool operator==(const tstring&) const;
  bool operator<(const tstring&) const;
  bool operator>(const tstring&) const;
  static constexpr size_t npos = -1;

  size_t find(char) const;
  size_t rfind(char) const;
  string to_string() const;
  operator string() const { return to_string(); }
};

string operator+(const tstring& a, const string& b);
