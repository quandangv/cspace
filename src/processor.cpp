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
  default: throw processor_error("Mod: Unknown operator: " + string{op});
  }
}

mod::mod(token_iterator& it, colorspace space) {
  logger::debug<scope>("Construct mod: " + it.input);
  // The argument for this setting follows the format: <component> <operator> <value>, ...
  // First token is the component
  if (!it.next_token_base<std::isalnum>()) {
    component = 0;
    op = 0;
    value = 0;
    return;
  }
  component = parse_component(it.token().data(), space);
  logger::debug<scope>("Mod component: '" + it.token() + "', value: " + to_string(component));

  // Next is the operator, which takes a single character
  if (!it.next_token_base<std::ispunct>() || it.token().empty())
    logger::warn("Interface-mod: Missing component operator and value in: " + it.input);
  op = it.token()[0];
  logger::debug<scope>("Mod operator: " + it.token());

  // Last is the value
  if (!it.next_token() || it.token().empty())
    logger::warn("Interface-mod: Missing value in: " + it.input);
  auto token = it.token();
  if (token.back() == ',') {
    token.pop_back();
  }
  if (!parse(token.data(), value))
    throw processor_error("mod: Can't parse numerical value: " + it.token());
}

processor::processor() {
  use_hex(true);
}

string processor::operate(double* data, bool have_alpha, colorspace from) {
  output_stream.str("");
  auto data_ptr = &data[(int)(have_alpha && alpha_first)];
  if (!modifications.empty()) {
    // Convert to the intermediate color space and do the modifications
    convert(data_ptr, from, inter);
    for(auto mod : modifications)
      mod.apply(data_ptr);
    convert(data_ptr, inter, target);
  } else
    // Convert to the destination color space directly
    convert(data_ptr, from, target);
  clamp(data_ptr, target);
  
  // Print data to output stream
  auto comp_count = component_count(target) + (int)have_alpha;
  if (output_stream.flags() & std::ios::hex) {
    for(int i = 0; i < comp_count; i++)
      output_stream << std::setw(2) << (int)round(data[i]*255);
  } else {
    output_stream << data[0];
    for(int i = 1; i < comp_count; i++)
      output_stream << separator << data[i];
  }
  return output_stream.str();
}

// Turns on or off hexedecimal output mode
bool processor::use_hex(bool value) {
  if (value) {
    output_stream << std::uppercase << std::setfill('0') << std::hex;
    target = colorspaces::rgb;
  } else {
    output_stream << std::dec << std::setfill(' ');
  }
  return value;
}

bool processor::use_hex() {
  return output_stream.flags() & std::ios::hex;
}

// Returns the state of the processor
string processor::get_state() {
  std::stringstream output;
  return output.str();
}

// Clears the state of the processor
void processor::clear() {
}

GLOBAL_NAMESPACE_END
