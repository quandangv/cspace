#include "processor.hpp"
#include "logger.hpp"

#include <iomanip>

#include "parse.hpp"

GLOBAL_NAMESPACE

DEFINE_ERROR(processor_error)
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
  default: throw processor_error("Mod: Unknown operator: " + std::to_string(op));
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
  m.component = parse_component(it.token().data(), space);
  logger::debug<scope>("Mod comp: " + it.token());

  // Next is the operator, which takes a single character
  if (!it.next_token_base<std::ispunct>() || it.token().empty())
    throw processor_error("Mod: Missing component operator and value in: " + it.input);
  m.op = it.token()[0];

  // Last is the value
  if (!it.next_token() || it.token().empty())
    throw processor_error("Mod: Missing value in: " + it.input);
  auto token = it.token();
  if (token.back() == ',')
    token.pop_back();
  if (!parse(token.data(), m.value))
    throw processor_error("mod: Can't parse numerical value: " + it.token());
}

mod::mod(string&& s, colorspace space) {
  token_iterator it(move(s));
  construct_mod(*this, it, space);
  if (it.next_token())
    throw processor_error("Mod: Excess tokens in initialization string: " + it.input);
}

mod::mod(token_iterator& it, colorspace space) {
  construct_mod(*this, it, space);
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

string processor::operate_hex(const string& hex) {
  double data[4];
  bool alpha;
  if (parse_hex(hex, &data[0], alpha)) {
    return operate(&data[0], alpha, colorspaces::rgb);
  } else
    return "";
}

mod& processor::add_modification(string&& s) {
  token_iterator it(move(s));
  mod* result;
  do {
    result = &modifications.emplace_back(it, inter);
  } while(it.have_token());
  return *result;
}

string processor::operate(double* data, bool have_alpha, colorspace from) {
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
