#include <string>
#include "x10_token.h"

X10_Token_t::X10_Token_t (std::string s, X10SourceCodePosition * p) : text(s), pos_info(p) {
    // Nothing to do here!
}

std::string X10_Token_t::getText() {
    return text;
}

X10SourceCodePosition *X10_Token_t::getSourcecodePosition() {
    return pos_info;
}
