#include "processor.hpp"
#include "logger.hpp"
#include "parse.hpp"
#include "token_iterator.hpp"

#include <iomanip>

GLOBAL_NAMESPACE

processor::processor(const processor& other) {
  inter = other.inter;
  separator = other.separator;
  modifications = other.modifications;
  alpha_first = other.alpha_first;
  output_stream.precision(other.output_stream.precision());
  use_hex(other.use_hex());
}

processor& processor::operator=(const processor& other) {
  inter = other.inter;
  separator = other.separator;
  modifications = other.modifications;
  alpha_first = other.alpha_first;
  output_stream.precision(other.output_stream.precision());
  use_hex(other.use_hex());
  return *this;
}

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

void construct_mod(mod& m, tstring& ts, colorspace space) {
  tstring word;
  // The argument for this setting follows the format: <component> <operator> <value>, ...
  // First token is the component
  if (word = get_token<std::isalnum>(ts); word.untouched()) {
    m.component = m.op = m.value = 0;
    return;
  }
  m.component = parse_component(word, space);

  // Next is the operator, which takes a single character
  if (word = get_token<std::ispunct>(ts); word.empty())
    throw mod::error("Missing component operator and value in: " + ts);
  m.op = word.front();

  // Last is the value
  if (word = get_word(ts); word.empty())
    throw mod::error("Missing value in: " + ts);
  if (word.back() == ',')
    word.erase_back();
  if (!parse(word, m.value))
    throw mod::error("Can't parse numerical value: " + word);
}

mod::mod(const string& s, colorspace space) {
  tstring ts;
  construct_mod(*this, ts, space);
  if (!ts.empty())
    throw error("Excess tokens in initialization string: " + s);
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

mod& processor::add_modification(const string& s) {
  mod* result;
  tstring ts{s};
  do {
    result = &modifications.emplace_back();
    construct_mod(*result, ts, inter);
  } while(!ts.empty());
  return *result;
}

void processor::silent_operate(const string& str) const {
  if (str.empty())
    return;
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
    auto pos = find(s, '('); 
    if (pos != tstring::npos && s.back() == ')') { 
      space = stospace(s.interval(0, pos)); 
      s.erase_front(pos + 1); 
      s.erase_back();
      size_t comp_count = 0;
      do {
        if (comp_count > 5)
          throw error("Too many color component: " + str);
        auto comma = find(s, ',');
        if (comma == tstring::npos)
          comma = s.size();
        if (!parse(s.interval(0, comma), data[comp_count++]))
          throw error("Invalid decimal number: " + s.interval(0, comma));
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
  silent_operate(&data[0], alpha, space);
}

void processor::silent_operate(double* data, bool have_alpha, colorspace from) const {
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
  print(output_stream, data, component_count(m_target) + (int)have_alpha);
}

void processor::print(ostream& os, double* data, int count) const {
  if (os.flags() & std::ios::hex) {
    os << '#';
    for(int i = 0; i < count; i++)
      os << std::setw(2) << lround(data[i]*255);
  } else {
    os << data[0];
    for(int i = 1; i < count; i++)
      os << separator << data[i];
  }
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

bool processor::use_hex() const {
  return output_stream.flags() & std::ios::hex;
}

GLOBAL_NAMESPACE_END
