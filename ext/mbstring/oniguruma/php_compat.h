#ifndef _PHP_MBREGEX_COMPAT_H
#define _PHP_MBREGEX_COMPAT_H

#define RegCharEncoding             php_mb_reg_char_encoding 
#define RegRegion                   php_mb_reg_region
#define RegDefaultCharEncoding      php_mb_reg_default_char_encoding
#define REG_MBLEN_TABLE             PHP_MBSTR_REG_MBLEN_TABLE
#define RegSyntaxType               php_mb_reg_syntax_type
#define RegOptionType               php_mb_reg_option_type
#define re_registers                php_mb_re_registers
#define RegErrorInfo                php_mb_reg_error_info
#define re_pattern_buffer           php_mb_re_pattern_buffer
#define regex_t                     php_mb_regex_t
#define regex_init                  php_mb_regex_init
#define regex_new                   php_mb_regex_new
#define regex_free                  php_mb_regex_free
#define regex_recompile             php_mb_regex_recompile
#define regex_search                php_mb_regex_search
#define regex_match                 php_mb_regex_match
#define regex_region_new            php_mb_regex_region_new
#define regex_region_free           php_mb_regex_region_free
#define regex_region_copy           php_mb_regex_region_copy
#define regex_region_clear          php_mb_regex_region_clear
#define regex_region_resize         php_mb_regex_region_resize
#define regex_name_to_group_numbers php_mb_regex_name_to_group_numbers
#define regex_foreach_names         php_mb_regex_foreach_names
#define regex_get_prev_char_head    php_mb_regex_get_prev_char_head
#define regex_get_left_adjust_char_head php_mb_get_left_adjust_char_head
#define regex_get_right_adjust_char_head php_mb_get_right_adjust_char_head
#define regex_set_default_trans_table php_mb_get_default_trans_table
#define regex_set_default_syntax    php_mb_regex_set_default_syntax
#define regex_end                   php_mb_regex_end
#define re_mbcinit                  php_mb_re_mbcinit
#define re_compile_pattern          php_mb_re_compile_pattern
#define re_recompile_pattern        php_mb_re_recompile_pattern
#define re_free_pattern             php_mb_re_free_pattern
#define re_adjust_startpos          php_mb_re_adjust_startpos
#define re_search                   php_mb_re_search
#define re_match                    php_mb_re_match
#define re_set_casetable            php_mb_re_set_casetable
#define php_mbregex_region_copy     php_mb_re_copy_registers
#define re_free_registers           php_mb_re_free_registers
#define register_info_type          php_mb_register_info_type
#define regex_error_code_to_str     php_mb_regex_error_code_to_str

#endif /* _PHP_MBREGEX_COMPAT_H */
