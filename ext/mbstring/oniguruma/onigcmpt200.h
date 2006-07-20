/**********************************************************************

  onigcmpt200.h - Oniguruma (regular expression library)

  Copyright (C) 2004  K.Kosako (kosako@sofnec.co.jp)

**********************************************************************/
#ifndef ONIGCMPT200_H
#define ONIGCMPT200_H

/* constants */
#define REG_MAX_ERROR_MESSAGE_LEN  ONIG_MAX_ERROR_MESSAGE_LEN

#define RegCharEncoding           OnigEncoding

#define REG_ENCODING_ASCII        ONIG_ENCODING_ASCII        
#define REG_ENCODING_ISO_8859_1   ONIG_ENCODING_ISO_8859_1
#define REG_ENCODING_ISO_8859_15  ONIG_ENCODING_ISO_8859_15
#define REG_ENCODING_UTF8         ONIG_ENCODING_UTF8
#define REG_ENCODING_EUC_JP       ONIG_ENCODING_EUC_JP
#define REG_ENCODING_SJIS         ONIG_ENCODING_SJIS
#define REG_ENCODING_BIG5         ONIG_ENCODING_BIG5
#define REG_ENCODING_UNDEF        ONIG_ENCODING_UNDEF

/* Don't use REGCODE_XXXX. (obsoleted) */
#define REGCODE_UNDEF         REG_ENCODING_UNDEF
#define REGCODE_ASCII         REG_ENCODING_ASCII
#define REGCODE_UTF8          REG_ENCODING_UTF8
#define REGCODE_EUCJP         REG_ENCODING_EUC_JP
#define REGCODE_SJIS          REG_ENCODING_SJIS

/* Don't use REGCODE_XXXX. (obsoleted) */
#define MBCTYPE_ASCII         RE_MBCTYPE_ASCII
#define MBCTYPE_EUC           RE_MBCTYPE_EUC
#define MBCTYPE_SJIS          RE_MBCTYPE_SJIS
#define MBCTYPE_UTF8          RE_MBCTYPE_UTF8

typedef unsigned char*   RegTransTableType;
#define RegOptionType    OnigOptionType
#define RegDistance      OnigDistance

#define REG_OPTION_DEFAULT      ONIG_OPTION_DEFAULT

/* options */
#define REG_OPTION_NONE                 ONIG_OPTION_NONE
#define REG_OPTION_SINGLELINE           ONIG_OPTION_SINGLELINE
#define REG_OPTION_MULTILINE            ONIG_OPTION_MULTILINE
#define REG_OPTION_IGNORECASE           ONIG_OPTION_IGNORECASE
#define REG_OPTION_EXTEND               ONIG_OPTION_EXTEND
#define REG_OPTION_FIND_LONGEST         ONIG_OPTION_FIND_LONGEST
#define REG_OPTION_FIND_NOT_EMPTY       ONIG_OPTION_FIND_NOT_EMPTY
#define REG_OPTION_NEGATE_SINGLELINE    ONIG_OPTION_NEGATE_SINGLELINE
#define REG_OPTION_DONT_CAPTURE_GROUP   ONIG_OPTION_DONT_CAPTURE_GROUP
#define REG_OPTION_CAPTURE_GROUP        ONIG_OPTION_CAPTURE_GROUP
#define REG_OPTION_NOTBOL               ONIG_OPTION_NOTBOL
#define REG_OPTION_NOTEOL               ONIG_OPTION_NOTEOL
#define REG_OPTION_POSIX_REGION         ONIG_OPTION_POSIX_REGION

#define REG_OPTION_ON                   ONIG_OPTION_ON 
#define REG_OPTION_OFF                  ONIG_OPTION_OFF
#define IS_REG_OPTION_ON                ONIG_IS_OPTION_ON

/* syntax */
#define RegSyntaxType OnigSyntaxType

#define RegSyntaxPosixBasic     OnigSyntaxPosixBasic
#define RegSyntaxPosixExtended  OnigSyntaxPosixExtended
#define RegSyntaxEmacs          OnigSyntaxEmacs
#define RegSyntaxGrep           OnigSyntaxGrep
#define RegSyntaxGnuRegex       OnigSyntaxGnuRegex
#define RegSyntaxJava           OnigSyntaxJava
#define RegSyntaxPerl           OnigSyntaxPerl
#define RegSyntaxRuby           OnigSyntaxRuby

#define REG_SYNTAX_POSIX_BASIC     ONIG_SYNTAX_POSIX_BASIC
#define REG_SYNTAX_POSIX_EXTENDED  ONIG_SYNTAX_POSIX_EXTENDED
#define REG_SYNTAX_EMACS           ONIG_SYNTAX_EMACS
#define REG_SYNTAX_GREP            ONIG_SYNTAX_GREP
#define REG_SYNTAX_GNU_REGEX       ONIG_SYNTAX_GNU_REGEX       
#define REG_SYNTAX_JAVA            ONIG_SYNTAX_JAVA
#define REG_SYNTAX_PERL            ONIG_SYNTAX_PERL
#define REG_SYNTAX_RUBY            ONIG_SYNTAX_RUBY

#define REG_SYNTAX_DEFAULT         ONIG_SYNTAX_DEFAULT
#define RegDefaultSyntax           OnigDefaultSyntax

/* syntax (operators) */
#define REG_SYN_OP_VARIABLE_META_CHARACTERS \
       ONIG_SYN_OP_VARIABLE_META_CHARACTERS
#define REG_SYN_OP_DOT_ANYCHAR \
       ONIG_SYN_OP_DOT_ANYCHAR
#define REG_SYN_OP_ASTERISK_ZERO_INF \
       ONIG_SYN_OP_ASTERISK_ZERO_INF
#define REG_SYN_OP_ESC_ASTERISK_ZERO_INF \
       ONIG_SYN_OP_ESC_ASTERISK_ZERO_INF
#define REG_SYN_OP_PLUS_ONE_INF \
       ONIG_SYN_OP_PLUS_ONE_INF
#define REG_SYN_OP_ESC_PLUS_ONE_INF \
       ONIG_SYN_OP_ESC_PLUS_ONE_INF
#define REG_SYN_OP_QMARK_ZERO_ONE \
       ONIG_SYN_OP_QMARK_ZERO_ONE
#define REG_SYN_OP_ESC_QMARK_ZERO_ONE \
       ONIG_SYN_OP_ESC_QMARK_ZERO_ONE
#define REG_SYN_OP_BRACE_INTERVAL \
       ONIG_SYN_OP_BRACE_INTERVAL
#define REG_SYN_OP_ESC_BRACE_INTERVAL \
       ONIG_SYN_OP_ESC_BRACE_INTERVAL
#define REG_SYN_OP_VBAR_ALT \
       ONIG_SYN_OP_VBAR_ALT
#define REG_SYN_OP_ESC_VBAR_ALT \
       ONIG_SYN_OP_ESC_VBAR_ALT
#define REG_SYN_OP_LPAREN_SUBEXP \
       ONIG_SYN_OP_LPAREN_SUBEXP
#define REG_SYN_OP_ESC_LPAREN_SUBEXP \
       ONIG_SYN_OP_ESC_LPAREN_SUBEXP
#define REG_SYN_OP_ESC_AZ_BUF_ANCHOR \
       ONIG_SYN_OP_ESC_AZ_BUF_ANCHOR
#define REG_SYN_OP_ESC_CAPITAL_G_BEGIN_ANCHOR \
       ONIG_SYN_OP_ESC_CAPITAL_G_BEGIN_ANCHOR
#define REG_SYN_OP_DECIMAL_BACKREF \
       ONIG_SYN_OP_DECIMAL_BACKREF
#define REG_SYN_OP_BRACKET_CC \
       ONIG_SYN_OP_BRACKET_CC
#define REG_SYN_OP_ESC_W_WORD \
       ONIG_SYN_OP_ESC_W_WORD
#define REG_SYN_OP_ESC_LTGT_WORD_BEGIN_END \
       ONIG_SYN_OP_ESC_LTGT_WORD_BEGIN_END
#define REG_SYN_OP_ESC_B_WORD_BOUND \
       ONIG_SYN_OP_ESC_B_WORD_BOUND
#define REG_SYN_OP_ESC_S_WHITE_SPACE \
       ONIG_SYN_OP_ESC_S_WHITE_SPACE
#define REG_SYN_OP_ESC_D_DIGIT \
       ONIG_SYN_OP_ESC_D_DIGIT
#define REG_SYN_OP_LINE_ANCHOR \
       ONIG_SYN_OP_LINE_ANCHOR
#define REG_SYN_OP_POSIX_BRACKET \
       ONIG_SYN_OP_POSIX_BRACKET
#define REG_SYN_OP_QMARK_NON_GREEDY \
       ONIG_SYN_OP_QMARK_NON_GREEDY
#define REG_SYN_OP_ESC_CONTROL_CHARS \
       ONIG_SYN_OP_ESC_CONTROL_CHARS
#define REG_SYN_OP_ESC_C_CONTROL \
       ONIG_SYN_OP_ESC_C_CONTROL
#define REG_SYN_OP_ESC_OCTAL3 \
       ONIG_SYN_OP_ESC_OCTAL3
#define REG_SYN_OP_ESC_X_HEX2 \
       ONIG_SYN_OP_ESC_X_HEX2
#define REG_SYN_OP_ESC_X_BRACE_HEX8 \
       ONIG_SYN_OP_ESC_X_BRACE_HEX8

#define REG_SYN_OP2_ESC_CAPITAL_Q_QUOTE \
       ONIG_SYN_OP2_ESC_CAPITAL_Q_QUOTE
#define REG_SYN_OP2_QMARK_GROUP_EFFECT \
       ONIG_SYN_OP2_QMARK_GROUP_EFFECT
#define REG_SYN_OP2_OPTION_PERL \
       ONIG_SYN_OP2_OPTION_PERL
#define REG_SYN_OP2_OPTION_RUBY \
       ONIG_SYN_OP2_OPTION_RUBY
#define REG_SYN_OP2_PLUS_POSSESSIVE_REPEAT \
       ONIG_SYN_OP2_PLUS_POSSESSIVE_REPEAT
#define REG_SYN_OP2_PLUS_POSSESSIVE_INTERVAL \
       ONIG_SYN_OP2_PLUS_POSSESSIVE_INTERVAL
#define REG_SYN_OP2_CCLASS_SET_OP \
       ONIG_SYN_OP2_CCLASS_SET_OP
#define REG_SYN_OP2_QMARK_LT_NAMED_GROUP \
       ONIG_SYN_OP2_QMARK_LT_NAMED_GROUP
#define REG_SYN_OP2_ESC_K_NAMED_BACKREF \
       ONIG_SYN_OP2_ESC_K_NAMED_BACKREF
#define REG_SYN_OP2_ESC_G_SUBEXP_CALL \
       ONIG_SYN_OP2_ESC_G_SUBEXP_CALL
#define REG_SYN_OP2_ATMARK_CAPTURE_HISTORY \
       ONIG_SYN_OP2_ATMARK_CAPTURE_HISTORY
#define REG_SYN_OP2_ESC_CAPITAL_C_BAR_CONTROL \
       ONIG_SYN_OP2_ESC_CAPITAL_C_BAR_CONTROL
#define REG_SYN_OP2_ESC_CAPITAL_M_BAR_META \
       ONIG_SYN_OP2_ESC_CAPITAL_M_BAR_META
#define REG_SYN_OP2_ESC_V_VTAB \
       ONIG_SYN_OP2_ESC_V_VTAB
#define REG_SYN_OP2_ESC_U_HEX4 \
       ONIG_SYN_OP2_ESC_U_HEX4
#define REG_SYN_OP2_ESC_GNU_BUF_ANCHOR \
       ONIG_SYN_OP2_ESC_GNU_BUF_ANCHOR

#define REG_SYN_CONTEXT_INDEP_ANCHORS \
       ONIG_SYN_CONTEXT_INDEP_ANCHORS
#define REG_SYN_CONTEXT_INDEP_REPEAT_OPS \
       ONIG_SYN_CONTEXT_INDEP_REPEAT_OPS
#define REG_SYN_CONTEXT_INVALID_REPEAT_OPS \
       ONIG_SYN_CONTEXT_INVALID_REPEAT_OPS
#define REG_SYN_ALLOW_UNMATCHED_CLOSE_SUBEXP \
       ONIG_SYN_ALLOW_UNMATCHED_CLOSE_SUBEXP
#define REG_SYN_ALLOW_INVALID_INTERVAL \
       ONIG_SYN_ALLOW_INVALID_INTERVAL
#define REG_SYN_STRICT_CHECK_BACKREF \
       ONIG_SYN_STRICT_CHECK_BACKREF
#define REG_SYN_DIFFERENT_LEN_ALT_LOOK_BEHIND \
       ONIG_SYN_DIFFERENT_LEN_ALT_LOOK_BEHIND
#define REG_SYN_CAPTURE_ONLY_NAMED_GROUP \
       ONIG_SYN_CAPTURE_ONLY_NAMED_GROUP
#define REG_SYN_ALLOW_MULTIPLEX_DEFINITION_NAME \
       ONIG_SYN_ALLOW_MULTIPLEX_DEFINITION_NAME

#define REG_SYN_NOT_NEWLINE_IN_NEGATIVE_CC \
       ONIG_SYN_NOT_NEWLINE_IN_NEGATIVE_CC
#define REG_SYN_BACKSLASH_ESCAPE_IN_CC \
       ONIG_SYN_BACKSLASH_ESCAPE_IN_CC
#define REG_SYN_ALLOW_EMPTY_RANGE_IN_CC \
       ONIG_SYN_ALLOW_EMPTY_RANGE_IN_CC
#define REG_SYN_ALLOW_DOUBLE_RANGE_OP_IN_CC \
       ONIG_SYN_ALLOW_DOUBLE_RANGE_OP_IN_CC
#define REG_SYN_WARN_CC_OP_NOT_ESCAPED \
       ONIG_SYN_WARN_CC_OP_NOT_ESCAPED
#define REG_SYN_WARN_REDUNDANT_NESTED_REPEAT \
       ONIG_SYN_WARN_REDUNDANT_NESTED_REPEAT

/* meta character specifiers (regex_set_meta_char()) */
#define REG_META_CHAR_ESCAPE            ONIG_META_CHAR_ESCAPE
#define REG_META_CHAR_ANYCHAR           ONIG_META_CHAR_ANYCHAR
#define REG_META_CHAR_ANYTIME           ONIG_META_CHAR_ANYTIME
#define REG_META_CHAR_ZERO_OR_ONE_TIME  ONIG_META_CHAR_ZERO_OR_ONE_TIME
#define REG_META_CHAR_ONE_OR_MORE_TIME  ONIG_META_CHAR_ONE_OR_MORE_TIME
#define REG_META_CHAR_ANYCHAR_ANYTIME   ONIG_META_CHAR_ANYCHAR_ANYTIME

#define REG_INEFFECTIVE_META_CHAR       ONIG_INEFFECTIVE_META_CHAR

/* error codes */
#define REG_IS_PATTERN_ERROR  ONIG_IS_PATTERN_ERROR
/* normal return */
#define REG_NORMAL               ONIG_NORMAL
#define REG_MISMATCH             ONIG_MISMATCH
#define REG_NO_SUPPORT_CONFIG    ONIG_NO_SUPPORT_CONFIG
/* internal error */
#define REGERR_MEMORY                    ONIGERR_MEMORY
#define REGERR_MATCH_STACK_LIMIT_OVER    ONIGERR_MATCH_STACK_LIMIT_OVER
#define REGERR_TYPE_BUG                  ONIGERR_TYPE_BUG
#define REGERR_PARSER_BUG                ONIGERR_PARSER_BUG
#define REGERR_STACK_BUG                 ONIGERR_STACK_BUG
#define REGERR_UNDEFINED_BYTECODE        ONIGERR_UNDEFINED_BYTECODE
#define REGERR_UNEXPECTED_BYTECODE       ONIGERR_UNEXPECTED_BYTECODE
#define REGERR_DEFAULT_ENCODING_IS_NOT_SETTED \
       ONIGERR_DEFAULT_ENCODING_IS_NOT_SETTED
#define REGERR_SPECIFIED_ENCODING_CANT_CONVERT_TO_WIDE_CHAR \
       ONIGERR_SPECIFIED_ENCODING_CANT_CONVERT_TO_WIDE_CHAR
/* general error */
#define REGERR_INVALID_ARGUMENT     ONIGERR_INVALID_ARGUMENT
/* errors related to thread */
#define REGERR_OVER_THREAD_PASS_LIMIT_COUNT \
       ONIGERR_OVER_THREAD_PASS_LIMIT_COUNT


/* must be smaller than BIT_STATUS_BITS_NUM (unsigned int * 8) */
#define REG_MAX_CAPTURE_HISTORY_GROUP      ONIG_MAX_CAPTURE_HISTORY_GROUP
#define REG_IS_CAPTURE_HISTORY_GROUP       ONIG_IS_CAPTURE_HISTORY_GROUP

#define REG_REGION_NOTPOS      ONIG_REGION_NOTPOS

#define RegRegion        OnigRegion
#define RegErrorInfo     OnigErrorInfo
#define RegRepeatRange   OnigRepeatRange

#define RegWarnFunc      OnigWarnFunc
#define regex_null_warn  onig_null_warn
#define REG_NULL_WARN    ONIG_NULL_WARN

/* regex_t state */
#define REG_STATE_NORMAL      ONIG_STATE_NORMAL
#define REG_STATE_SEARCHING   ONIG_STATE_SEARCHING
#define REG_STATE_COMPILING   ONIG_STATE_COMPILING
#define REG_STATE_MODIFY      ONIG_STATE_MODIFY

#define REG_STATE     ONIG_STATE

/* Oniguruma Native API */
#define regex_init                   onig_init
#define regex_error_code_to_str      onig_error_code_to_str
#define regex_set_warn_func          onig_set_warn_func      
#define regex_set_verb_warn_func     onig_set_verb_warn_func
#define regex_new                    onig_new
#define regex_free                   onig_free
#define regex_recompile              onig_recompile
#define regex_search                 onig_search
#define regex_match                  onig_match
#define regex_region_new             onig_region_new
#define regex_region_free            onig_region_free
#define regex_region_copy            onig_region_copy
#define regex_region_clear           onig_region_clear
#define regex_region_resize          onig_region_resize
#define regex_name_to_group_numbers  onig_name_to_group_numbers
#define regex_name_to_backref_number onig_name_to_backref_number
#define regex_foreach_name           onig_foreach_name 
#define regex_number_of_names        onig_number_of_names
#define regex_get_encoding           onig_get_encoding
#define regex_get_options            onig_get_options
#define regex_get_syntax             onig_get_syntax
#define regex_set_default_syntax     onig_set_default_syntax
#define regex_copy_syntax            onig_copy_syntax 
#define regex_set_meta_char          onig_set_meta_char
#define regex_end                    onig_end
#define regex_version                onig_version

/* encoding API */
#define enc_get_prev_char_head          onigenc_get_prev_char_head
#define enc_get_left_adjust_char_head   onigenc_get_left_adjust_char_head 
#define enc_get_right_adjust_char_head  onigenc_get_right_adjust_char_head
/* obsoleted API */
#define regex_get_prev_char_head         onigenc_get_prev_char_head
#define regex_get_left_adjust_char_head  onigenc_get_left_adjust_char_head
#define regex_get_right_adjust_char_head onigenc_get_right_adjust_char_head

#endif /* ONIGCMPT200_H */
