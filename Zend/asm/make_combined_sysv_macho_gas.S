/*
            Copyright Sergue E. Leontiev 2013.
   Distributed under the Boost Software License, Version 1.0.
      (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

// Stub file for universal binary

#if defined(__i386__)
    #include "make_i386_sysv_macho_gas.S"
#elif defined(__x86_64__)
    #include "make_x86_64_sysv_macho_gas.S"
#elif defined(__ppc__)
    #include "make_ppc32_sysv_macho_gas.S"
#elif defined(__ppc64__)
    #include "make_ppc64_sysv_macho_gas.S"
#elif defined(__arm__)
    #include "make_arm_aapcs_macho_gas.S"
#elif defined(__arm64__)
    #include "make_arm64_aapcs_macho_gas.S"
#else
    #error "No arch's"
#endif
