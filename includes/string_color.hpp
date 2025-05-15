#pragma once

#include "colors.hpp"

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <cstdarg>

static const char opening_brackets = '{';
static const char closing_brackets = '}';
static const char replacer = '%';
static const char r_string = 's';
static const char r_int = 'i';
static const char r_double = 'd';
static const char r_char = 'c';

std::string StrColor (std::string str, ...);