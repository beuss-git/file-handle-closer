#pragma once

#include "nt_defs.hpp"

namespace nt {
extern tNtQuerySystemInformation NtQuerySystemInformation;
extern tNtQueryObject NtQueryObject;

extern bool resolve_functions();
}  // namespace nt
