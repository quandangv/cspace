#include "processor.hpp"
#include "logger.hpp"
#include "parse.hpp"
#include "token_iterator.hpp"

#include <iomanip>

GLOBAL_NAMESPACE

constexpr char scope[] = "processor";

// Apply the right operator to the right component, with the right value
void mod::apply(double* data) const {
  auto& target = data[component];
  switch(op) {
  case '+': target += value; break;
  case '-': target -= value; break;
  case '*': target *= value; break;
  case '/': target /= value; break;
  case '=': target = value; break;
  default: throw error("Unknown operator: " + string{op});
  }
}

void construct_mod(mod& m, token_iterator& it, colorspace space) {
  logger::debug<scope>("Construct mod: " + it.input);
  // The argument for this setting follows the format: <component> <operator> <value>, ...
  // First token is the component
  if (!it.next_token_base<std::isalnum>()) {
    m.component = m.op = m.value = 0;
    return;
  }
  m.component = parse_component(it.token(), space);

  // Next is the operator, which takes a single character
  if (!it.next_token_base<std::ispunct>() || it.token().empty())
    throw mod::error("Missing component operator and value in: " + it.input);
  m.op = it.token().front();

  // Last is the value
  if (!it.next_token() || it.token().empty())
    throw mod::error("Missing value in: " + it.input);
  auto& token = it.token();
  if (token.back() == ',')
    token.erase_back();
  if (!parse(token, m.value))
    throw mod::error("Can't parse numerical value: " + it.token().to_string());
}

mod::mod(string&& s, colorspace space) {
  token_iterator it(move(s));
  construct_mod(*this, it, space);
  if (it.next_token())
    throw error("Excess tokens in initialization string: " + it.input);
}

processor::processor() {
  use_hex(true);
}

colorspace processor::target() const {
  return m_target;
}

colorspace processor::target(colorspace s) {
  if (s == m_target) return m_target;
  // If any output color space is specified, disable hex mode, which is only meant to be used in RGB color space
  use_hex(false);
  return m_target = s;
}

string processor::operate(const string& str) const {
  if (str.empty())
    return "";
  double data[5];
  bool alpha;
  colorspace space;
  tstring s(str);
  if (s[0] == '#') {
    s.erase_front();
    if (!parse_hex(s, &data[0], alpha))
      throw error("Invalid hexedecimal color code: " + str);
    space = colorspaces::rgb;
  } else {
    auto pos = s.find('('); 
    if (pos != tstring::npos && s.back() == ')') { 
      space = stospace(s.substr(0, pos).to_string()); 
      s.erase_front(pos + 1); 
      s.erase_back();
      size_t comp_count = 0;
      do {
        if (comp_count > 5)
          throw error("Too many color component: " + str);
        auto comma = s.find(',');
        if (comma == tstring::npos)
          comma = s.size();
        if (!parse(s.substr(0, comma), data[comp_count++]))
          throw error("Invalid decimal number: " + s.substr(0, comma).to_string());
        s.erase_front(comma + 1);
      } while(!s.empty());
      size_t supposed_count = component_count(space);
      if (comp_count > supposed_count) {
        if (comp_count > supposed_count + 1)
          throw error("Wrong number of color components: " + str);
        alpha = true;
      } else alpha = false;
    } else throw error("Unknown operate input: " + str);
  }
  return operate(&data[0], alpha, space);
}

mod& processor::add_modification(string&& s) {
  token_iterator it(move(s));
  mod* result;
  do {
    result = &modifications.emplace_back();
    construct_mod(*result, it, inter);
  } while(it.have_token());
  return *result;
}

string processor::operate(double* data, bool have_alpha, colorspace from) const {
  auto data_ptr = data + (int)(have_alpha && alpha_first);
  if (!modifications.empty()) {
    // Convert to the intermediate color space and do the modifications
    convert(data_ptr, from, inter);
    for(auto& mod : modifications)
      mod.apply(data_ptr);
    convert(data_ptr, inter, m_target);
  } else
    // Convert to the destination color space directly
    convert(data_ptr, from, m_target);
  clamp(data_ptr, m_target);
  
  // Print data to output stream
  auto comp_count = component_count(m_target) + (int)have_alpha;
  if (output_stream.flags() & std::ios::hex) {
    output_stream.str("");
    output_stream << '#';
    for(int i = 0; i < comp_count; i++)
      output_stream << std::setw(2) << (int)round(data[i]*255);
    return output_stream.str();
  } else
    return print(data, comp_count);
}

string processor::print(double* data, int count) const {
  output_stream.str("");
  output_stream << data[0];
  for(int i = 1; i < count; i++)
    output_stream << separator << data[i];
  return output_stream.str();
}

// Turns on or off hexedecimal output mode
bool processor::use_hex(bool value) {
  if (value) {
    output_stream << std::uppercase << std::setfill('0') << std::hex;
    m_target = colorspaces::rgb;
  } else {
    output_stream << std::dec << std::setfill(' ');
  }
  return value;
}

bool processor::use_hex() {
  return output_stream.flags() & std::ios::hex;
}

GLOBAL_NAMESPACE_END
