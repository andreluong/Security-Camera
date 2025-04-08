file(REMOVE_RECURSE
  "include/generated/zephyr/syscalls"
  "CMakeFiles/syscall_list_h_target"
  "include/generated/zephyr/syscall_dispatch.c"
  "include/generated/zephyr/syscall_exports_llext.c"
  "include/generated/zephyr/syscall_list.h"
  "syscall_weakdefs_llext.c"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/syscall_list_h_target.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
