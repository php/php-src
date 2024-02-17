<?php

/** @generate-class-entries */

class IntlChar
{
    /** @cvalue U_UNICODE_VERSION */
    public const string UNICODE_VERSION = UNKNOWN;
    /** @cvalue UCHAR_MIN_VALUE */
    public const int CODEPOINT_MIN = UNKNOWN;
    /** @cvalue UCHAR_MAX_VALUE */
    public const int CODEPOINT_MAX = UNKNOWN;
    /** @cvalue U_NO_NUMERIC_VALUE */
    public const float NO_NUMERIC_VALUE = UNKNOWN;

    /** @cvalue UCHAR_ALPHABETIC */
    public const int PROPERTY_ALPHABETIC = UNKNOWN;
    /** @cvalue UCHAR_BINARY_START */
    public const int PROPERTY_BINARY_START = UNKNOWN;
    /** @cvalue UCHAR_ASCII_HEX_DIGIT */
    public const int PROPERTY_ASCII_HEX_DIGIT = UNKNOWN;
    /** @cvalue UCHAR_BIDI_CONTROL */
    public const int PROPERTY_BIDI_CONTROL = UNKNOWN;
    /** @cvalue UCHAR_BIDI_MIRRORED */
    public const int PROPERTY_BIDI_MIRRORED = UNKNOWN;
    /** @cvalue UCHAR_DASH */
    public const int PROPERTY_DASH = UNKNOWN;
    /** @cvalue UCHAR_DEFAULT_IGNORABLE_CODE_POINT */
    public const int PROPERTY_DEFAULT_IGNORABLE_CODE_POINT = UNKNOWN;
    /** @cvalue UCHAR_DEPRECATED */
    public const int PROPERTY_DEPRECATED = UNKNOWN;
    /** @cvalue UCHAR_DIACRITIC */
    public const int PROPERTY_DIACRITIC = UNKNOWN;
    /** @cvalue UCHAR_EXTENDER */
    public const int PROPERTY_EXTENDER = UNKNOWN;
    /** @cvalue UCHAR_FULL_COMPOSITION_EXCLUSION */
    public const int PROPERTY_FULL_COMPOSITION_EXCLUSION = UNKNOWN;
    /** @cvalue UCHAR_GRAPHEME_BASE */
    public const int PROPERTY_GRAPHEME_BASE = UNKNOWN;
    /** @cvalue UCHAR_GRAPHEME_EXTEND */
    public const int PROPERTY_GRAPHEME_EXTEND = UNKNOWN;
    /** @cvalue UCHAR_GRAPHEME_LINK */
    public const int PROPERTY_GRAPHEME_LINK = UNKNOWN;
    /** @cvalue UCHAR_HEX_DIGIT */
    public const int PROPERTY_HEX_DIGIT = UNKNOWN;
    /** @cvalue UCHAR_HYPHEN */
    public const int PROPERTY_HYPHEN = UNKNOWN;
    /** @cvalue UCHAR_ID_CONTINUE */
    public const int PROPERTY_ID_CONTINUE = UNKNOWN;
    /** @cvalue UCHAR_ID_START */
    public const int PROPERTY_ID_START = UNKNOWN;
    /** @cvalue UCHAR_IDEOGRAPHIC */
    public const int PROPERTY_IDEOGRAPHIC = UNKNOWN;
    /** @cvalue UCHAR_IDS_BINARY_OPERATOR */
    public const int PROPERTY_IDS_BINARY_OPERATOR = UNKNOWN;
    /** @cvalue UCHAR_IDS_TRINARY_OPERATOR */
    public const int PROPERTY_IDS_TRINARY_OPERATOR = UNKNOWN;
#if U_ICU_VERSION_MAJOR_NUM >= 74
    /** @cvalue UCHAR_IDS_UNARY_OPERATOR */
    public const int PROPERTY_IDS_UNARY_OPERATOR = UNKNOWN;
    /** @cvalue UCHAR_ID_COMPAT_MATH_START */
    public const int PROPERTY_ID_COMPT_MATH_START = UNKNOWN;
    /** @cvalue UCHAR_ID_COMPAT_MATH_CONTINUE */
    public const int PROPERTY_ID_COMPT_MATH_CONTINUE = UNKNOWN;
#endif
    /** @cvalue UCHAR_JOIN_CONTROL */
    public const int PROPERTY_JOIN_CONTROL = UNKNOWN;
    /** @cvalue UCHAR_LOGICAL_ORDER_EXCEPTION */
    public const int PROPERTY_LOGICAL_ORDER_EXCEPTION = UNKNOWN;
    /** @cvalue UCHAR_LOWERCASE */
    public const int PROPERTY_LOWERCASE = UNKNOWN;
    /** @cvalue UCHAR_MATH */
    public const int PROPERTY_MATH = UNKNOWN;
    /** @cvalue UCHAR_NONCHARACTER_CODE_POINT */
    public const int PROPERTY_NONCHARACTER_CODE_POINT = UNKNOWN;
    /** @cvalue UCHAR_QUOTATION_MARK */
    public const int PROPERTY_QUOTATION_MARK = UNKNOWN;
    /** @cvalue UCHAR_RADICAL */
    public const int PROPERTY_RADICAL = UNKNOWN;
    /** @cvalue UCHAR_SOFT_DOTTED */
    public const int PROPERTY_SOFT_DOTTED = UNKNOWN;
    /** @cvalue UCHAR_TERMINAL_PUNCTUATION */
    public const int PROPERTY_TERMINAL_PUNCTUATION = UNKNOWN;
    /** @cvalue UCHAR_UNIFIED_IDEOGRAPH */
    public const int PROPERTY_UNIFIED_IDEOGRAPH = UNKNOWN;
    /** @cvalue UCHAR_UPPERCASE */
    public const int PROPERTY_UPPERCASE = UNKNOWN;
    /** @cvalue UCHAR_WHITE_SPACE */
    public const int PROPERTY_WHITE_SPACE = UNKNOWN;
    /** @cvalue UCHAR_XID_CONTINUE */
    public const int PROPERTY_XID_CONTINUE = UNKNOWN;
    /** @cvalue UCHAR_XID_START */
    public const int PROPERTY_XID_START = UNKNOWN;
    /** @cvalue UCHAR_CASE_SENSITIVE */
    public const int PROPERTY_CASE_SENSITIVE = UNKNOWN;
    /** @cvalue UCHAR_S_TERM */
    public const int PROPERTY_S_TERM = UNKNOWN;
    /** @cvalue UCHAR_VARIATION_SELECTOR */
    public const int PROPERTY_VARIATION_SELECTOR = UNKNOWN;
    /** @cvalue UCHAR_NFD_INERT */
    public const int PROPERTY_NFD_INERT = UNKNOWN;
    /** @cvalue UCHAR_NFKD_INERT */
    public const int PROPERTY_NFKD_INERT = UNKNOWN;
    /** @cvalue UCHAR_NFC_INERT */
    public const int PROPERTY_NFC_INERT = UNKNOWN;
    /** @cvalue UCHAR_NFKC_INERT */
    public const int PROPERTY_NFKC_INERT = UNKNOWN;
    /** @cvalue UCHAR_SEGMENT_STARTER */
    public const int PROPERTY_SEGMENT_STARTER = UNKNOWN;
    /** @cvalue UCHAR_PATTERN_SYNTAX */
    public const int PROPERTY_PATTERN_SYNTAX = UNKNOWN;
    /** @cvalue UCHAR_PATTERN_WHITE_SPACE */
    public const int PROPERTY_PATTERN_WHITE_SPACE = UNKNOWN;
    /** @cvalue UCHAR_POSIX_ALNUM */
    public const int PROPERTY_POSIX_ALNUM = UNKNOWN;
    /** @cvalue UCHAR_POSIX_BLANK */
    public const int PROPERTY_POSIX_BLANK = UNKNOWN;
    /** @cvalue UCHAR_POSIX_GRAPH */
    public const int PROPERTY_POSIX_GRAPH = UNKNOWN;
    /** @cvalue UCHAR_POSIX_PRINT */
    public const int PROPERTY_POSIX_PRINT = UNKNOWN;
    /** @cvalue UCHAR_POSIX_XDIGIT */
    public const int PROPERTY_POSIX_XDIGIT = UNKNOWN;
    /** @cvalue UCHAR_CASED */
    public const int PROPERTY_CASED = UNKNOWN;
    /** @cvalue UCHAR_CASE_IGNORABLE */
    public const int PROPERTY_CASE_IGNORABLE = UNKNOWN;
    /** @cvalue UCHAR_CHANGES_WHEN_LOWERCASED */
    public const int PROPERTY_CHANGES_WHEN_LOWERCASED = UNKNOWN;
    /** @cvalue UCHAR_CHANGES_WHEN_UPPERCASED */
    public const int PROPERTY_CHANGES_WHEN_UPPERCASED = UNKNOWN;
    /** @cvalue UCHAR_CHANGES_WHEN_TITLECASED */
    public const int PROPERTY_CHANGES_WHEN_TITLECASED = UNKNOWN;
    /** @cvalue UCHAR_CHANGES_WHEN_CASEFOLDED */
    public const int PROPERTY_CHANGES_WHEN_CASEFOLDED = UNKNOWN;
    /** @cvalue UCHAR_CHANGES_WHEN_CASEMAPPED */
    public const int PROPERTY_CHANGES_WHEN_CASEMAPPED = UNKNOWN;
    /** @cvalue UCHAR_CHANGES_WHEN_NFKC_CASEFOLDED */
    public const int PROPERTY_CHANGES_WHEN_NFKC_CASEFOLDED = UNKNOWN;
    /** @cvalue UCHAR_BINARY_LIMIT */
    public const int PROPERTY_BINARY_LIMIT = UNKNOWN;
    /** @cvalue UCHAR_BIDI_CLASS */
    public const int PROPERTY_BIDI_CLASS = UNKNOWN;
    /** @cvalue UCHAR_INT_START */
    public const int PROPERTY_INT_START = UNKNOWN;
    /** @cvalue UCHAR_BLOCK */
    public const int PROPERTY_BLOCK = UNKNOWN;
    /** @cvalue UCHAR_CANONICAL_COMBINING_CLASS */
    public const int PROPERTY_CANONICAL_COMBINING_CLASS = UNKNOWN;
    /** @cvalue UCHAR_DECOMPOSITION_TYPE */
    public const int PROPERTY_DECOMPOSITION_TYPE = UNKNOWN;
    /** @cvalue UCHAR_EAST_ASIAN_WIDTH */
    public const int PROPERTY_EAST_ASIAN_WIDTH = UNKNOWN;
    /** @cvalue UCHAR_GENERAL_CATEGORY */
    public const int PROPERTY_GENERAL_CATEGORY = UNKNOWN;
    /** @cvalue UCHAR_JOINING_GROUP */
    public const int PROPERTY_JOINING_GROUP = UNKNOWN;
    /** @cvalue UCHAR_JOINING_TYPE */
    public const int PROPERTY_JOINING_TYPE = UNKNOWN;
    /** @cvalue UCHAR_LINE_BREAK */
    public const int PROPERTY_LINE_BREAK = UNKNOWN;
    /** @cvalue UCHAR_NUMERIC_TYPE */
    public const int PROPERTY_NUMERIC_TYPE = UNKNOWN;
    /** @cvalue UCHAR_SCRIPT */
    public const int PROPERTY_SCRIPT = UNKNOWN;
    /** @cvalue UCHAR_HANGUL_SYLLABLE_TYPE */
    public const int PROPERTY_HANGUL_SYLLABLE_TYPE = UNKNOWN;
    /** @cvalue UCHAR_NFD_QUICK_CHECK */
    public const int PROPERTY_NFD_QUICK_CHECK = UNKNOWN;
    /** @cvalue UCHAR_NFKD_QUICK_CHECK */
    public const int PROPERTY_NFKD_QUICK_CHECK = UNKNOWN;
    /** @cvalue UCHAR_NFC_QUICK_CHECK */
    public const int PROPERTY_NFC_QUICK_CHECK = UNKNOWN;
    /** @cvalue UCHAR_NFKC_QUICK_CHECK */
    public const int PROPERTY_NFKC_QUICK_CHECK = UNKNOWN;
    /** @cvalue UCHAR_LEAD_CANONICAL_COMBINING_CLASS */
    public const int PROPERTY_LEAD_CANONICAL_COMBINING_CLASS = UNKNOWN;
    /** @cvalue UCHAR_TRAIL_CANONICAL_COMBINING_CLASS */
    public const int PROPERTY_TRAIL_CANONICAL_COMBINING_CLASS = UNKNOWN;
    /** @cvalue UCHAR_GRAPHEME_CLUSTER_BREAK */
    public const int PROPERTY_GRAPHEME_CLUSTER_BREAK = UNKNOWN;
    /** @cvalue UCHAR_SENTENCE_BREAK */
    public const int PROPERTY_SENTENCE_BREAK = UNKNOWN;
    /** @cvalue UCHAR_WORD_BREAK */
    public const int PROPERTY_WORD_BREAK = UNKNOWN;
#if U_ICU_VERSION_MAJOR_NUM >= 52
    /** @cvalue UCHAR_BIDI_PAIRED_BRACKET_TYPE */
    public const int PROPERTY_BIDI_PAIRED_BRACKET_TYPE = UNKNOWN;
#endif
    /** @cvalue UCHAR_INT_LIMIT */
    public const int PROPERTY_INT_LIMIT = UNKNOWN;
    /** @cvalue UCHAR_GENERAL_CATEGORY_MASK */
    public const int PROPERTY_GENERAL_CATEGORY_MASK = UNKNOWN;
    /** @cvalue UCHAR_MASK_START */
    public const int PROPERTY_MASK_START = UNKNOWN;
    /** @cvalue UCHAR_MASK_LIMIT */
    public const int PROPERTY_MASK_LIMIT = UNKNOWN;
    /** @cvalue UCHAR_NUMERIC_VALUE */
    public const int PROPERTY_NUMERIC_VALUE = UNKNOWN;
    /** @cvalue UCHAR_DOUBLE_START */
    public const int PROPERTY_DOUBLE_START = UNKNOWN;
    /** @cvalue UCHAR_DOUBLE_LIMIT */
    public const int PROPERTY_DOUBLE_LIMIT = UNKNOWN;
    /** @cvalue UCHAR_AGE */
    public const int PROPERTY_AGE = UNKNOWN;
    /** @cvalue UCHAR_STRING_START */
    public const int PROPERTY_STRING_START = UNKNOWN;
    /** @cvalue UCHAR_BIDI_MIRRORING_GLYPH */
    public const int PROPERTY_BIDI_MIRRORING_GLYPH = UNKNOWN;
    /** @cvalue UCHAR_CASE_FOLDING */
    public const int PROPERTY_CASE_FOLDING = UNKNOWN;
    /** @cvalue UCHAR_ISO_COMMENT */
    public const int PROPERTY_ISO_COMMENT = UNKNOWN;
    /** @cvalue UCHAR_LOWERCASE_MAPPING */
    public const int PROPERTY_LOWERCASE_MAPPING = UNKNOWN;
    /** @cvalue UCHAR_NAME */
    public const int PROPERTY_NAME = UNKNOWN;
    /** @cvalue UCHAR_SIMPLE_CASE_FOLDING */
    public const int PROPERTY_SIMPLE_CASE_FOLDING = UNKNOWN;
    /** @cvalue UCHAR_SIMPLE_LOWERCASE_MAPPING */
    public const int PROPERTY_SIMPLE_LOWERCASE_MAPPING = UNKNOWN;
    /** @cvalue UCHAR_SIMPLE_TITLECASE_MAPPING */
    public const int PROPERTY_SIMPLE_TITLECASE_MAPPING = UNKNOWN;
    /** @cvalue UCHAR_SIMPLE_UPPERCASE_MAPPING */
    public const int PROPERTY_SIMPLE_UPPERCASE_MAPPING = UNKNOWN;
    /** @cvalue UCHAR_TITLECASE_MAPPING */
    public const int PROPERTY_TITLECASE_MAPPING = UNKNOWN;
    /** @cvalue UCHAR_UNICODE_1_NAME */
    public const int PROPERTY_UNICODE_1_NAME = UNKNOWN;
    /** @cvalue UCHAR_UPPERCASE_MAPPING */
    public const int PROPERTY_UPPERCASE_MAPPING = UNKNOWN;
#if U_ICU_VERSION_MAJOR_NUM >= 52
    /** @cvalue UCHAR_BIDI_PAIRED_BRACKET */
    public const int PROPERTY_BIDI_PAIRED_BRACKET = UNKNOWN;
#endif
    /** @cvalue UCHAR_STRING_LIMIT */
    public const int PROPERTY_STRING_LIMIT = UNKNOWN;
    /** @cvalue UCHAR_SCRIPT_EXTENSIONS */
    public const int PROPERTY_SCRIPT_EXTENSIONS = UNKNOWN;
    /** @cvalue UCHAR_OTHER_PROPERTY_START */
    public const int PROPERTY_OTHER_PROPERTY_START = UNKNOWN;
    /** @cvalue UCHAR_OTHER_PROPERTY_LIMIT */
    public const int PROPERTY_OTHER_PROPERTY_LIMIT = UNKNOWN;
    /** @cvalue UCHAR_INVALID_CODE */
    public const int PROPERTY_INVALID_CODE = UNKNOWN;

    /** @cvalue U_UNASSIGNED */
    public const int CHAR_CATEGORY_UNASSIGNED = UNKNOWN;
    /** @cvalue U_GENERAL_OTHER_TYPES */
    public const int CHAR_CATEGORY_GENERAL_OTHER_TYPES = UNKNOWN;
    /** @cvalue U_UPPERCASE_LETTER */
    public const int CHAR_CATEGORY_UPPERCASE_LETTER = UNKNOWN;
    /** @cvalue U_LOWERCASE_LETTER */
    public const int CHAR_CATEGORY_LOWERCASE_LETTER = UNKNOWN;
    /** @cvalue U_TITLECASE_LETTER */
    public const int CHAR_CATEGORY_TITLECASE_LETTER = UNKNOWN;
    /** @cvalue U_MODIFIER_LETTER */
    public const int CHAR_CATEGORY_MODIFIER_LETTER = UNKNOWN;
    /** @cvalue U_OTHER_LETTER */
    public const int CHAR_CATEGORY_OTHER_LETTER = UNKNOWN;
    /** @cvalue U_NON_SPACING_MARK */
    public const int CHAR_CATEGORY_NON_SPACING_MARK = UNKNOWN;
    /** @cvalue U_ENCLOSING_MARK */
    public const int CHAR_CATEGORY_ENCLOSING_MARK = UNKNOWN;
    /** @cvalue U_COMBINING_SPACING_MARK */
    public const int CHAR_CATEGORY_COMBINING_SPACING_MARK = UNKNOWN;
    /** @cvalue U_DECIMAL_DIGIT_NUMBER */
    public const int CHAR_CATEGORY_DECIMAL_DIGIT_NUMBER = UNKNOWN;
    /** @cvalue U_LETTER_NUMBER */
    public const int CHAR_CATEGORY_LETTER_NUMBER = UNKNOWN;
    /** @cvalue U_OTHER_NUMBER */
    public const int CHAR_CATEGORY_OTHER_NUMBER = UNKNOWN;
    /** @cvalue U_SPACE_SEPARATOR */
    public const int CHAR_CATEGORY_SPACE_SEPARATOR = UNKNOWN;
    /** @cvalue U_LINE_SEPARATOR */
    public const int CHAR_CATEGORY_LINE_SEPARATOR = UNKNOWN;
    /** @cvalue U_PARAGRAPH_SEPARATOR */
    public const int CHAR_CATEGORY_PARAGRAPH_SEPARATOR = UNKNOWN;
    /** @cvalue U_CONTROL_CHAR */
    public const int CHAR_CATEGORY_CONTROL_CHAR = UNKNOWN;
    /** @cvalue U_FORMAT_CHAR */
    public const int CHAR_CATEGORY_FORMAT_CHAR = UNKNOWN;
    /** @cvalue U_PRIVATE_USE_CHAR */
    public const int CHAR_CATEGORY_PRIVATE_USE_CHAR = UNKNOWN;
    /** @cvalue U_SURROGATE */
    public const int CHAR_CATEGORY_SURROGATE = UNKNOWN;
    /** @cvalue U_DASH_PUNCTUATION */
    public const int CHAR_CATEGORY_DASH_PUNCTUATION = UNKNOWN;
    /** @cvalue U_START_PUNCTUATION */
    public const int CHAR_CATEGORY_START_PUNCTUATION = UNKNOWN;
    /** @cvalue U_END_PUNCTUATION */
    public const int CHAR_CATEGORY_END_PUNCTUATION = UNKNOWN;
    /** @cvalue U_CONNECTOR_PUNCTUATION */
    public const int CHAR_CATEGORY_CONNECTOR_PUNCTUATION = UNKNOWN;
    /** @cvalue U_OTHER_PUNCTUATION */
    public const int CHAR_CATEGORY_OTHER_PUNCTUATION = UNKNOWN;
    /** @cvalue U_MATH_SYMBOL */
    public const int CHAR_CATEGORY_MATH_SYMBOL = UNKNOWN;
    /** @cvalue U_CURRENCY_SYMBOL */
    public const int CHAR_CATEGORY_CURRENCY_SYMBOL = UNKNOWN;
    /** @cvalue U_MODIFIER_SYMBOL */
    public const int CHAR_CATEGORY_MODIFIER_SYMBOL = UNKNOWN;
    /** @cvalue U_OTHER_SYMBOL */
    public const int CHAR_CATEGORY_OTHER_SYMBOL = UNKNOWN;
    /** @cvalue U_INITIAL_PUNCTUATION */
    public const int CHAR_CATEGORY_INITIAL_PUNCTUATION = UNKNOWN;
    /** @cvalue U_FINAL_PUNCTUATION */
    public const int CHAR_CATEGORY_FINAL_PUNCTUATION = UNKNOWN;
    /** @cvalue U_CHAR_CATEGORY_COUNT */
    public const int CHAR_CATEGORY_CHAR_CATEGORY_COUNT = UNKNOWN;

    /** @cvalue U_LEFT_TO_RIGHT */
    public const int CHAR_DIRECTION_LEFT_TO_RIGHT = UNKNOWN;
    /** @cvalue U_RIGHT_TO_LEFT */
    public const int CHAR_DIRECTION_RIGHT_TO_LEFT = UNKNOWN;
    /** @cvalue U_EUROPEAN_NUMBER */
    public const int CHAR_DIRECTION_EUROPEAN_NUMBER = UNKNOWN;
    /** @cvalue U_EUROPEAN_NUMBER_SEPARATOR */
    public const int CHAR_DIRECTION_EUROPEAN_NUMBER_SEPARATOR = UNKNOWN;
    /** @cvalue U_EUROPEAN_NUMBER_TERMINATOR */
    public const int CHAR_DIRECTION_EUROPEAN_NUMBER_TERMINATOR = UNKNOWN;
    /** @cvalue U_ARABIC_NUMBER */
    public const int CHAR_DIRECTION_ARABIC_NUMBER = UNKNOWN;
    /** @cvalue U_COMMON_NUMBER_SEPARATOR */
    public const int CHAR_DIRECTION_COMMON_NUMBER_SEPARATOR = UNKNOWN;
    /** @cvalue U_BLOCK_SEPARATOR */
    public const int CHAR_DIRECTION_BLOCK_SEPARATOR = UNKNOWN;
    /** @cvalue U_SEGMENT_SEPARATOR */
    public const int CHAR_DIRECTION_SEGMENT_SEPARATOR = UNKNOWN;
    /** @cvalue U_WHITE_SPACE_NEUTRAL */
    public const int CHAR_DIRECTION_WHITE_SPACE_NEUTRAL = UNKNOWN;
    /** @cvalue U_OTHER_NEUTRAL */
    public const int CHAR_DIRECTION_OTHER_NEUTRAL = UNKNOWN;
    /** @cvalue U_LEFT_TO_RIGHT_EMBEDDING */
    public const int CHAR_DIRECTION_LEFT_TO_RIGHT_EMBEDDING = UNKNOWN;
    /** @cvalue U_LEFT_TO_RIGHT_OVERRIDE */
    public const int CHAR_DIRECTION_LEFT_TO_RIGHT_OVERRIDE = UNKNOWN;
    /** @cvalue U_RIGHT_TO_LEFT_ARABIC */
    public const int CHAR_DIRECTION_RIGHT_TO_LEFT_ARABIC = UNKNOWN;
    /** @cvalue U_RIGHT_TO_LEFT_EMBEDDING */
    public const int CHAR_DIRECTION_RIGHT_TO_LEFT_EMBEDDING = UNKNOWN;
    /** @cvalue U_RIGHT_TO_LEFT_OVERRIDE */
    public const int CHAR_DIRECTION_RIGHT_TO_LEFT_OVERRIDE = UNKNOWN;
    /** @cvalue U_POP_DIRECTIONAL_FORMAT */
    public const int CHAR_DIRECTION_POP_DIRECTIONAL_FORMAT = UNKNOWN;
    /** @cvalue U_DIR_NON_SPACING_MARK */
    public const int CHAR_DIRECTION_DIR_NON_SPACING_MARK = UNKNOWN;
    /** @cvalue U_BOUNDARY_NEUTRAL */
    public const int CHAR_DIRECTION_BOUNDARY_NEUTRAL = UNKNOWN;
#if U_ICU_VERSION_MAJOR_NUM >= 52
    /** @cvalue U_FIRST_STRONG_ISOLATE */
    public const int CHAR_DIRECTION_FIRST_STRONG_ISOLATE = UNKNOWN;
    /** @cvalue U_LEFT_TO_RIGHT_ISOLATE */
    public const int CHAR_DIRECTION_LEFT_TO_RIGHT_ISOLATE = UNKNOWN;
    /** @cvalue U_RIGHT_TO_LEFT_ISOLATE */
    public const int CHAR_DIRECTION_RIGHT_TO_LEFT_ISOLATE = UNKNOWN;
    /** @cvalue U_POP_DIRECTIONAL_ISOLATE */
    public const int CHAR_DIRECTION_POP_DIRECTIONAL_ISOLATE = UNKNOWN;
#endif
    /** @cvalue U_CHAR_DIRECTION_COUNT */
    public const int CHAR_DIRECTION_CHAR_DIRECTION_COUNT = UNKNOWN;

    /** @cvalue UBLOCK_NO_BLOCK */
    public const int BLOCK_CODE_NO_BLOCK = UNKNOWN;
    /** @cvalue UBLOCK_BASIC_LATIN */
    public const int BLOCK_CODE_BASIC_LATIN = UNKNOWN;
    /** @cvalue UBLOCK_LATIN_1_SUPPLEMENT */
    public const int BLOCK_CODE_LATIN_1_SUPPLEMENT = UNKNOWN;
    /** @cvalue UBLOCK_LATIN_EXTENDED_A */
    public const int BLOCK_CODE_LATIN_EXTENDED_A = UNKNOWN;
    /** @cvalue UBLOCK_LATIN_EXTENDED_B */
    public const int BLOCK_CODE_LATIN_EXTENDED_B = UNKNOWN;
    /** @cvalue UBLOCK_IPA_EXTENSIONS */
    public const int BLOCK_CODE_IPA_EXTENSIONS = UNKNOWN;
    /** @cvalue UBLOCK_SPACING_MODIFIER_LETTERS */
    public const int BLOCK_CODE_SPACING_MODIFIER_LETTERS = UNKNOWN;
    /** @cvalue UBLOCK_COMBINING_DIACRITICAL_MARKS */
    public const int BLOCK_CODE_COMBINING_DIACRITICAL_MARKS = UNKNOWN;
    /** @cvalue UBLOCK_GREEK */
    public const int BLOCK_CODE_GREEK = UNKNOWN;
    /** @cvalue UBLOCK_CYRILLIC */
    public const int BLOCK_CODE_CYRILLIC = UNKNOWN;
    /** @cvalue UBLOCK_ARMENIAN */
    public const int BLOCK_CODE_ARMENIAN = UNKNOWN;
    /** @cvalue UBLOCK_HEBREW */
    public const int BLOCK_CODE_HEBREW = UNKNOWN;
    /** @cvalue UBLOCK_ARABIC */
    public const int BLOCK_CODE_ARABIC = UNKNOWN;
    /** @cvalue UBLOCK_SYRIAC */
    public const int BLOCK_CODE_SYRIAC = UNKNOWN;
    /** @cvalue UBLOCK_THAANA */
    public const int BLOCK_CODE_THAANA = UNKNOWN;
    /** @cvalue UBLOCK_DEVANAGARI */
    public const int BLOCK_CODE_DEVANAGARI = UNKNOWN;
    /** @cvalue UBLOCK_BENGALI */
    public const int BLOCK_CODE_BENGALI = UNKNOWN;
    /** @cvalue UBLOCK_GURMUKHI */
    public const int BLOCK_CODE_GURMUKHI = UNKNOWN;
    /** @cvalue UBLOCK_GUJARATI */
    public const int BLOCK_CODE_GUJARATI = UNKNOWN;
    /** @cvalue UBLOCK_ORIYA */
    public const int BLOCK_CODE_ORIYA = UNKNOWN;
    /** @cvalue UBLOCK_TAMIL */
    public const int BLOCK_CODE_TAMIL = UNKNOWN;
    /** @cvalue UBLOCK_TELUGU */
    public const int BLOCK_CODE_TELUGU = UNKNOWN;
    /** @cvalue UBLOCK_KANNADA */
    public const int BLOCK_CODE_KANNADA = UNKNOWN;
    /** @cvalue UBLOCK_MALAYALAM */
    public const int BLOCK_CODE_MALAYALAM = UNKNOWN;
    /** @cvalue UBLOCK_SINHALA */
    public const int BLOCK_CODE_SINHALA = UNKNOWN;
    /** @cvalue UBLOCK_THAI */
    public const int BLOCK_CODE_THAI = UNKNOWN;
    /** @cvalue UBLOCK_LAO */
    public const int BLOCK_CODE_LAO = UNKNOWN;
    /** @cvalue UBLOCK_TIBETAN */
    public const int BLOCK_CODE_TIBETAN = UNKNOWN;
    /** @cvalue UBLOCK_MYANMAR */
    public const int BLOCK_CODE_MYANMAR = UNKNOWN;
    /** @cvalue UBLOCK_GEORGIAN */
    public const int BLOCK_CODE_GEORGIAN = UNKNOWN;
    /** @cvalue UBLOCK_HANGUL_JAMO */
    public const int BLOCK_CODE_HANGUL_JAMO = UNKNOWN;
    /** @cvalue UBLOCK_ETHIOPIC */
    public const int BLOCK_CODE_ETHIOPIC = UNKNOWN;
    /** @cvalue UBLOCK_CHEROKEE */
    public const int BLOCK_CODE_CHEROKEE = UNKNOWN;
    /** @cvalue UBLOCK_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS */
    public const int BLOCK_CODE_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS = UNKNOWN;
    /** @cvalue UBLOCK_OGHAM */
    public const int BLOCK_CODE_OGHAM = UNKNOWN;
    /** @cvalue UBLOCK_RUNIC */
    public const int BLOCK_CODE_RUNIC = UNKNOWN;
    /** @cvalue UBLOCK_KHMER */
    public const int BLOCK_CODE_KHMER = UNKNOWN;
    /** @cvalue UBLOCK_MONGOLIAN */
    public const int BLOCK_CODE_MONGOLIAN = UNKNOWN;
    /** @cvalue UBLOCK_LATIN_EXTENDED_ADDITIONAL */
    public const int BLOCK_CODE_LATIN_EXTENDED_ADDITIONAL = UNKNOWN;
    /** @cvalue UBLOCK_GREEK_EXTENDED */
    public const int BLOCK_CODE_GREEK_EXTENDED = UNKNOWN;
    /** @cvalue UBLOCK_GENERAL_PUNCTUATION */
    public const int BLOCK_CODE_GENERAL_PUNCTUATION = UNKNOWN;
    /** @cvalue UBLOCK_SUPERSCRIPTS_AND_SUBSCRIPTS */
    public const int BLOCK_CODE_SUPERSCRIPTS_AND_SUBSCRIPTS = UNKNOWN;
    /** @cvalue UBLOCK_CURRENCY_SYMBOLS */
    public const int BLOCK_CODE_CURRENCY_SYMBOLS = UNKNOWN;
    /** @cvalue UBLOCK_COMBINING_MARKS_FOR_SYMBOLS */
    public const int BLOCK_CODE_COMBINING_MARKS_FOR_SYMBOLS = UNKNOWN;
    /** @cvalue UBLOCK_LETTERLIKE_SYMBOLS */
    public const int BLOCK_CODE_LETTERLIKE_SYMBOLS = UNKNOWN;
    /** @cvalue UBLOCK_NUMBER_FORMS */
    public const int BLOCK_CODE_NUMBER_FORMS = UNKNOWN;
    /** @cvalue UBLOCK_ARROWS */
    public const int BLOCK_CODE_ARROWS = UNKNOWN;
    /** @cvalue UBLOCK_MATHEMATICAL_OPERATORS */
    public const int BLOCK_CODE_MATHEMATICAL_OPERATORS = UNKNOWN;
    /** @cvalue UBLOCK_MISCELLANEOUS_TECHNICAL */
    public const int BLOCK_CODE_MISCELLANEOUS_TECHNICAL = UNKNOWN;
    /** @cvalue UBLOCK_CONTROL_PICTURES */
    public const int BLOCK_CODE_CONTROL_PICTURES = UNKNOWN;
    /** @cvalue UBLOCK_OPTICAL_CHARACTER_RECOGNITION */
    public const int BLOCK_CODE_OPTICAL_CHARACTER_RECOGNITION = UNKNOWN;
    /** @cvalue UBLOCK_ENCLOSED_ALPHANUMERICS */
    public const int BLOCK_CODE_ENCLOSED_ALPHANUMERICS = UNKNOWN;
    /** @cvalue UBLOCK_BOX_DRAWING */
    public const int BLOCK_CODE_BOX_DRAWING = UNKNOWN;
    /** @cvalue UBLOCK_BLOCK_ELEMENTS */
    public const int BLOCK_CODE_BLOCK_ELEMENTS = UNKNOWN;
    /** @cvalue UBLOCK_GEOMETRIC_SHAPES */
    public const int BLOCK_CODE_GEOMETRIC_SHAPES = UNKNOWN;
    /** @cvalue UBLOCK_MISCELLANEOUS_SYMBOLS */
    public const int BLOCK_CODE_MISCELLANEOUS_SYMBOLS = UNKNOWN;
    /** @cvalue UBLOCK_DINGBATS */
    public const int BLOCK_CODE_DINGBATS = UNKNOWN;
    /** @cvalue UBLOCK_BRAILLE_PATTERNS */
    public const int BLOCK_CODE_BRAILLE_PATTERNS = UNKNOWN;
    /** @cvalue UBLOCK_CJK_RADICALS_SUPPLEMENT */
    public const int BLOCK_CODE_CJK_RADICALS_SUPPLEMENT = UNKNOWN;
    /** @cvalue UBLOCK_KANGXI_RADICALS */
    public const int BLOCK_CODE_KANGXI_RADICALS = UNKNOWN;
    /** @cvalue UBLOCK_IDEOGRAPHIC_DESCRIPTION_CHARACTERS */
    public const int BLOCK_CODE_IDEOGRAPHIC_DESCRIPTION_CHARACTERS = UNKNOWN;
    /** @cvalue UBLOCK_CJK_SYMBOLS_AND_PUNCTUATION */
    public const int BLOCK_CODE_CJK_SYMBOLS_AND_PUNCTUATION = UNKNOWN;
    /** @cvalue UBLOCK_HIRAGANA */
    public const int BLOCK_CODE_HIRAGANA = UNKNOWN;
    /** @cvalue UBLOCK_KATAKANA */
    public const int BLOCK_CODE_KATAKANA = UNKNOWN;
    /** @cvalue UBLOCK_BOPOMOFO */
    public const int BLOCK_CODE_BOPOMOFO = UNKNOWN;
    /** @cvalue UBLOCK_HANGUL_COMPATIBILITY_JAMO */
    public const int BLOCK_CODE_HANGUL_COMPATIBILITY_JAMO = UNKNOWN;
    /** @cvalue UBLOCK_KANBUN */
    public const int BLOCK_CODE_KANBUN = UNKNOWN;
    /** @cvalue UBLOCK_BOPOMOFO_EXTENDED */
    public const int BLOCK_CODE_BOPOMOFO_EXTENDED = UNKNOWN;
    /** @cvalue UBLOCK_ENCLOSED_CJK_LETTERS_AND_MONTHS */
    public const int BLOCK_CODE_ENCLOSED_CJK_LETTERS_AND_MONTHS = UNKNOWN;
    /** @cvalue UBLOCK_CJK_COMPATIBILITY */
    public const int BLOCK_CODE_CJK_COMPATIBILITY = UNKNOWN;
    /** @cvalue UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A */
    public const int BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A = UNKNOWN;
    /** @cvalue UBLOCK_CJK_UNIFIED_IDEOGRAPHS */
    public const int BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS = UNKNOWN;
    /** @cvalue UBLOCK_YI_SYLLABLES */
    public const int BLOCK_CODE_YI_SYLLABLES = UNKNOWN;
    /** @cvalue UBLOCK_YI_RADICALS */
    public const int BLOCK_CODE_YI_RADICALS = UNKNOWN;
    /** @cvalue UBLOCK_HANGUL_SYLLABLES */
    public const int BLOCK_CODE_HANGUL_SYLLABLES = UNKNOWN;
    /** @cvalue UBLOCK_HIGH_SURROGATES */
    public const int BLOCK_CODE_HIGH_SURROGATES = UNKNOWN;
    /** @cvalue UBLOCK_HIGH_PRIVATE_USE_SURROGATES */
    public const int BLOCK_CODE_HIGH_PRIVATE_USE_SURROGATES = UNKNOWN;
    /** @cvalue UBLOCK_LOW_SURROGATES */
    public const int BLOCK_CODE_LOW_SURROGATES = UNKNOWN;
    /** @cvalue UBLOCK_PRIVATE_USE_AREA */
    public const int BLOCK_CODE_PRIVATE_USE_AREA = UNKNOWN;
    /** @cvalue UBLOCK_PRIVATE_USE */
    public const int BLOCK_CODE_PRIVATE_USE = UNKNOWN;
    /** @cvalue UBLOCK_CJK_COMPATIBILITY_IDEOGRAPHS */
    public const int BLOCK_CODE_CJK_COMPATIBILITY_IDEOGRAPHS = UNKNOWN;
    /** @cvalue UBLOCK_ALPHABETIC_PRESENTATION_FORMS */
    public const int BLOCK_CODE_ALPHABETIC_PRESENTATION_FORMS = UNKNOWN;
    /** @cvalue UBLOCK_ARABIC_PRESENTATION_FORMS_A */
    public const int BLOCK_CODE_ARABIC_PRESENTATION_FORMS_A = UNKNOWN;
    /** @cvalue UBLOCK_COMBINING_HALF_MARKS */
    public const int BLOCK_CODE_COMBINING_HALF_MARKS = UNKNOWN;
    /** @cvalue UBLOCK_CJK_COMPATIBILITY_FORMS */
    public const int BLOCK_CODE_CJK_COMPATIBILITY_FORMS = UNKNOWN;
    /** @cvalue UBLOCK_SMALL_FORM_VARIANTS */
    public const int BLOCK_CODE_SMALL_FORM_VARIANTS = UNKNOWN;
    /** @cvalue UBLOCK_ARABIC_PRESENTATION_FORMS_B */
    public const int BLOCK_CODE_ARABIC_PRESENTATION_FORMS_B = UNKNOWN;
    /** @cvalue UBLOCK_SPECIALS */
    public const int BLOCK_CODE_SPECIALS = UNKNOWN;
    /** @cvalue UBLOCK_HALFWIDTH_AND_FULLWIDTH_FORMS */
    public const int BLOCK_CODE_HALFWIDTH_AND_FULLWIDTH_FORMS = UNKNOWN;
    /** @cvalue UBLOCK_OLD_ITALIC */
    public const int BLOCK_CODE_OLD_ITALIC = UNKNOWN;
    /** @cvalue UBLOCK_GOTHIC */
    public const int BLOCK_CODE_GOTHIC = UNKNOWN;
    /** @cvalue UBLOCK_DESERET */
    public const int BLOCK_CODE_DESERET = UNKNOWN;
    /** @cvalue UBLOCK_BYZANTINE_MUSICAL_SYMBOLS */
    public const int BLOCK_CODE_BYZANTINE_MUSICAL_SYMBOLS = UNKNOWN;
    /** @cvalue UBLOCK_MUSICAL_SYMBOLS */
    public const int BLOCK_CODE_MUSICAL_SYMBOLS = UNKNOWN;
    /** @cvalue UBLOCK_MATHEMATICAL_ALPHANUMERIC_SYMBOLS */
    public const int BLOCK_CODE_MATHEMATICAL_ALPHANUMERIC_SYMBOLS = UNKNOWN;
    /** @cvalue UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B */
    public const int BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B = UNKNOWN;
    /** @cvalue UBLOCK_CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT */
    public const int BLOCK_CODE_CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT = UNKNOWN;
    /** @cvalue UBLOCK_TAGS */
    public const int BLOCK_CODE_TAGS = UNKNOWN;
    /** @cvalue UBLOCK_CYRILLIC_SUPPLEMENT */
    public const int BLOCK_CODE_CYRILLIC_SUPPLEMENT = UNKNOWN;
    /** @cvalue UBLOCK_CYRILLIC_SUPPLEMENTARY */
    public const int BLOCK_CODE_CYRILLIC_SUPPLEMENTARY = UNKNOWN;
    /** @cvalue UBLOCK_TAGALOG */
    public const int BLOCK_CODE_TAGALOG = UNKNOWN;
    /** @cvalue UBLOCK_HANUNOO */
    public const int BLOCK_CODE_HANUNOO = UNKNOWN;
    /** @cvalue UBLOCK_BUHID */
    public const int BLOCK_CODE_BUHID = UNKNOWN;
    /** @cvalue UBLOCK_TAGBANWA */
    public const int BLOCK_CODE_TAGBANWA = UNKNOWN;
    /** @cvalue UBLOCK_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A */
    public const int BLOCK_CODE_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A = UNKNOWN;
    /** @cvalue UBLOCK_SUPPLEMENTAL_ARROWS_A */
    public const int BLOCK_CODE_SUPPLEMENTAL_ARROWS_A = UNKNOWN;
    /** @cvalue UBLOCK_SUPPLEMENTAL_ARROWS_B */
    public const int BLOCK_CODE_SUPPLEMENTAL_ARROWS_B = UNKNOWN;
    /** @cvalue UBLOCK_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B */
    public const int BLOCK_CODE_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B = UNKNOWN;
    /** @cvalue UBLOCK_SUPPLEMENTAL_MATHEMATICAL_OPERATORS */
    public const int BLOCK_CODE_SUPPLEMENTAL_MATHEMATICAL_OPERATORS = UNKNOWN;
    /** @cvalue UBLOCK_KATAKANA_PHONETIC_EXTENSIONS */
    public const int BLOCK_CODE_KATAKANA_PHONETIC_EXTENSIONS = UNKNOWN;
    /** @cvalue UBLOCK_VARIATION_SELECTORS */
    public const int BLOCK_CODE_VARIATION_SELECTORS = UNKNOWN;
    /** @cvalue UBLOCK_SUPPLEMENTARY_PRIVATE_USE_AREA_A */
    public const int BLOCK_CODE_SUPPLEMENTARY_PRIVATE_USE_AREA_A = UNKNOWN;
    /** @cvalue UBLOCK_SUPPLEMENTARY_PRIVATE_USE_AREA_B */
    public const int BLOCK_CODE_SUPPLEMENTARY_PRIVATE_USE_AREA_B = UNKNOWN;
    /** @cvalue UBLOCK_LIMBU */
    public const int BLOCK_CODE_LIMBU = UNKNOWN;
    /** @cvalue UBLOCK_TAI_LE */
    public const int BLOCK_CODE_TAI_LE = UNKNOWN;
    /** @cvalue UBLOCK_KHMER_SYMBOLS */
    public const int BLOCK_CODE_KHMER_SYMBOLS = UNKNOWN;
    /** @cvalue UBLOCK_PHONETIC_EXTENSIONS */
    public const int BLOCK_CODE_PHONETIC_EXTENSIONS = UNKNOWN;
    /** @cvalue UBLOCK_MISCELLANEOUS_SYMBOLS_AND_ARROWS */
    public const int BLOCK_CODE_MISCELLANEOUS_SYMBOLS_AND_ARROWS = UNKNOWN;
    /** @cvalue UBLOCK_YIJING_HEXAGRAM_SYMBOLS */
    public const int BLOCK_CODE_YIJING_HEXAGRAM_SYMBOLS = UNKNOWN;
    /** @cvalue UBLOCK_LINEAR_B_SYLLABARY */
    public const int BLOCK_CODE_LINEAR_B_SYLLABARY = UNKNOWN;
    /** @cvalue UBLOCK_LINEAR_B_IDEOGRAMS */
    public const int BLOCK_CODE_LINEAR_B_IDEOGRAMS = UNKNOWN;
    /** @cvalue UBLOCK_AEGEAN_NUMBERS */
    public const int BLOCK_CODE_AEGEAN_NUMBERS = UNKNOWN;
    /** @cvalue UBLOCK_UGARITIC */
    public const int BLOCK_CODE_UGARITIC = UNKNOWN;
    /** @cvalue UBLOCK_SHAVIAN */
    public const int BLOCK_CODE_SHAVIAN = UNKNOWN;
    /** @cvalue UBLOCK_OSMANYA */
    public const int BLOCK_CODE_OSMANYA = UNKNOWN;
    /** @cvalue UBLOCK_CYPRIOT_SYLLABARY */
    public const int BLOCK_CODE_CYPRIOT_SYLLABARY = UNKNOWN;
    /** @cvalue UBLOCK_TAI_XUAN_JING_SYMBOLS */
    public const int BLOCK_CODE_TAI_XUAN_JING_SYMBOLS = UNKNOWN;
    /** @cvalue UBLOCK_VARIATION_SELECTORS_SUPPLEMENT */
    public const int BLOCK_CODE_VARIATION_SELECTORS_SUPPLEMENT = UNKNOWN;
    /** @cvalue UBLOCK_ANCIENT_GREEK_MUSICAL_NOTATION */
    public const int BLOCK_CODE_ANCIENT_GREEK_MUSICAL_NOTATION = UNKNOWN;
    /** @cvalue UBLOCK_ANCIENT_GREEK_NUMBERS */
    public const int BLOCK_CODE_ANCIENT_GREEK_NUMBERS = UNKNOWN;
    /** @cvalue UBLOCK_ARABIC_SUPPLEMENT */
    public const int BLOCK_CODE_ARABIC_SUPPLEMENT = UNKNOWN;
    /** @cvalue UBLOCK_BUGINESE */
    public const int BLOCK_CODE_BUGINESE = UNKNOWN;
    /** @cvalue UBLOCK_CJK_STROKES */
    public const int BLOCK_CODE_CJK_STROKES = UNKNOWN;
    /** @cvalue UBLOCK_COMBINING_DIACRITICAL_MARKS_SUPPLEMENT */
    public const int BLOCK_CODE_COMBINING_DIACRITICAL_MARKS_SUPPLEMENT = UNKNOWN;
    /** @cvalue UBLOCK_COPTIC */
    public const int BLOCK_CODE_COPTIC = UNKNOWN;
    /** @cvalue UBLOCK_ETHIOPIC_EXTENDED */
    public const int BLOCK_CODE_ETHIOPIC_EXTENDED = UNKNOWN;
    /** @cvalue UBLOCK_ETHIOPIC_SUPPLEMENT */
    public const int BLOCK_CODE_ETHIOPIC_SUPPLEMENT = UNKNOWN;
    /** @cvalue UBLOCK_GEORGIAN_SUPPLEMENT */
    public const int BLOCK_CODE_GEORGIAN_SUPPLEMENT = UNKNOWN;
    /** @cvalue UBLOCK_GLAGOLITIC */
    public const int BLOCK_CODE_GLAGOLITIC = UNKNOWN;
    /** @cvalue UBLOCK_KHAROSHTHI */
    public const int BLOCK_CODE_KHAROSHTHI = UNKNOWN;
    /** @cvalue UBLOCK_MODIFIER_TONE_LETTERS */
    public const int BLOCK_CODE_MODIFIER_TONE_LETTERS = UNKNOWN;
    /** @cvalue UBLOCK_NEW_TAI_LUE */
    public const int BLOCK_CODE_NEW_TAI_LUE = UNKNOWN;
    /** @cvalue UBLOCK_OLD_PERSIAN */
    public const int BLOCK_CODE_OLD_PERSIAN = UNKNOWN;
    /** @cvalue UBLOCK_PHONETIC_EXTENSIONS_SUPPLEMENT */
    public const int BLOCK_CODE_PHONETIC_EXTENSIONS_SUPPLEMENT = UNKNOWN;
    /** @cvalue UBLOCK_SUPPLEMENTAL_PUNCTUATION */
    public const int BLOCK_CODE_SUPPLEMENTAL_PUNCTUATION = UNKNOWN;
    /** @cvalue UBLOCK_SYLOTI_NAGRI */
    public const int BLOCK_CODE_SYLOTI_NAGRI = UNKNOWN;
    /** @cvalue UBLOCK_TIFINAGH */
    public const int BLOCK_CODE_TIFINAGH = UNKNOWN;
    /** @cvalue UBLOCK_VERTICAL_FORMS */
    public const int BLOCK_CODE_VERTICAL_FORMS = UNKNOWN;
    /** @cvalue UBLOCK_NKO */
    public const int BLOCK_CODE_NKO = UNKNOWN;
    /** @cvalue UBLOCK_BALINESE */
    public const int BLOCK_CODE_BALINESE = UNKNOWN;
    /** @cvalue UBLOCK_LATIN_EXTENDED_C */
    public const int BLOCK_CODE_LATIN_EXTENDED_C = UNKNOWN;
    /** @cvalue UBLOCK_LATIN_EXTENDED_D */
    public const int BLOCK_CODE_LATIN_EXTENDED_D = UNKNOWN;
    /** @cvalue UBLOCK_PHAGS_PA */
    public const int BLOCK_CODE_PHAGS_PA = UNKNOWN;
    /** @cvalue UBLOCK_PHOENICIAN */
    public const int BLOCK_CODE_PHOENICIAN = UNKNOWN;
    /** @cvalue UBLOCK_CUNEIFORM */
    public const int BLOCK_CODE_CUNEIFORM = UNKNOWN;
    /** @cvalue UBLOCK_CUNEIFORM_NUMBERS_AND_PUNCTUATION */
    public const int BLOCK_CODE_CUNEIFORM_NUMBERS_AND_PUNCTUATION = UNKNOWN;
    /** @cvalue UBLOCK_COUNTING_ROD_NUMERALS */
    public const int BLOCK_CODE_COUNTING_ROD_NUMERALS = UNKNOWN;
    /** @cvalue UBLOCK_SUNDANESE */
    public const int BLOCK_CODE_SUNDANESE = UNKNOWN;
    /** @cvalue UBLOCK_LEPCHA */
    public const int BLOCK_CODE_LEPCHA = UNKNOWN;
    /** @cvalue UBLOCK_OL_CHIKI */
    public const int BLOCK_CODE_OL_CHIKI = UNKNOWN;
    /** @cvalue UBLOCK_CYRILLIC_EXTENDED_A */
    public const int BLOCK_CODE_CYRILLIC_EXTENDED_A = UNKNOWN;
    /** @cvalue UBLOCK_VAI */
    public const int BLOCK_CODE_VAI = UNKNOWN;
    /** @cvalue UBLOCK_CYRILLIC_EXTENDED_B */
    public const int BLOCK_CODE_CYRILLIC_EXTENDED_B = UNKNOWN;
    /** @cvalue UBLOCK_SAURASHTRA */
    public const int BLOCK_CODE_SAURASHTRA = UNKNOWN;
    /** @cvalue UBLOCK_KAYAH_LI */
    public const int BLOCK_CODE_KAYAH_LI = UNKNOWN;
    /** @cvalue UBLOCK_REJANG */
    public const int BLOCK_CODE_REJANG = UNKNOWN;
    /** @cvalue UBLOCK_CHAM */
    public const int BLOCK_CODE_CHAM = UNKNOWN;
    /** @cvalue UBLOCK_ANCIENT_SYMBOLS */
    public const int BLOCK_CODE_ANCIENT_SYMBOLS = UNKNOWN;
    /** @cvalue UBLOCK_PHAISTOS_DISC */
    public const int BLOCK_CODE_PHAISTOS_DISC = UNKNOWN;
    /** @cvalue UBLOCK_LYCIAN */
    public const int BLOCK_CODE_LYCIAN = UNKNOWN;
    /** @cvalue UBLOCK_CARIAN */
    public const int BLOCK_CODE_CARIAN = UNKNOWN;
    /** @cvalue UBLOCK_LYDIAN */
    public const int BLOCK_CODE_LYDIAN = UNKNOWN;
    /** @cvalue UBLOCK_MAHJONG_TILES */
    public const int BLOCK_CODE_MAHJONG_TILES = UNKNOWN;
    /** @cvalue UBLOCK_DOMINO_TILES */
    public const int BLOCK_CODE_DOMINO_TILES = UNKNOWN;
    /** @cvalue UBLOCK_SAMARITAN */
    public const int BLOCK_CODE_SAMARITAN = UNKNOWN;
    /** @cvalue UBLOCK_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDED */
    public const int BLOCK_CODE_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDED = UNKNOWN;
    /** @cvalue UBLOCK_TAI_THAM */
    public const int BLOCK_CODE_TAI_THAM = UNKNOWN;
    /** @cvalue UBLOCK_VEDIC_EXTENSIONS */
    public const int BLOCK_CODE_VEDIC_EXTENSIONS = UNKNOWN;
    /** @cvalue UBLOCK_LISU */
    public const int BLOCK_CODE_LISU = UNKNOWN;
    /** @cvalue UBLOCK_BAMUM */
    public const int BLOCK_CODE_BAMUM = UNKNOWN;
    /** @cvalue UBLOCK_COMMON_INDIC_NUMBER_FORMS */
    public const int BLOCK_CODE_COMMON_INDIC_NUMBER_FORMS = UNKNOWN;
    /** @cvalue UBLOCK_DEVANAGARI_EXTENDED */
    public const int BLOCK_CODE_DEVANAGARI_EXTENDED = UNKNOWN;
    /** @cvalue UBLOCK_HANGUL_JAMO_EXTENDED_A */
    public const int BLOCK_CODE_HANGUL_JAMO_EXTENDED_A = UNKNOWN;
    /** @cvalue UBLOCK_JAVANESE */
    public const int BLOCK_CODE_JAVANESE = UNKNOWN;
    /** @cvalue UBLOCK_MYANMAR_EXTENDED_A */
    public const int BLOCK_CODE_MYANMAR_EXTENDED_A = UNKNOWN;
    /** @cvalue UBLOCK_TAI_VIET */
    public const int BLOCK_CODE_TAI_VIET = UNKNOWN;
    /** @cvalue UBLOCK_MEETEI_MAYEK */
    public const int BLOCK_CODE_MEETEI_MAYEK = UNKNOWN;
    /** @cvalue UBLOCK_HANGUL_JAMO_EXTENDED_B */
    public const int BLOCK_CODE_HANGUL_JAMO_EXTENDED_B = UNKNOWN;
    /** @cvalue UBLOCK_IMPERIAL_ARAMAIC */
    public const int BLOCK_CODE_IMPERIAL_ARAMAIC = UNKNOWN;
    /** @cvalue UBLOCK_OLD_SOUTH_ARABIAN */
    public const int BLOCK_CODE_OLD_SOUTH_ARABIAN = UNKNOWN;
    /** @cvalue UBLOCK_AVESTAN */
    public const int BLOCK_CODE_AVESTAN = UNKNOWN;
    /** @cvalue UBLOCK_INSCRIPTIONAL_PARTHIAN */
    public const int BLOCK_CODE_INSCRIPTIONAL_PARTHIAN = UNKNOWN;
    /** @cvalue UBLOCK_INSCRIPTIONAL_PAHLAVI */
    public const int BLOCK_CODE_INSCRIPTIONAL_PAHLAVI = UNKNOWN;
    /** @cvalue UBLOCK_OLD_TURKIC */
    public const int BLOCK_CODE_OLD_TURKIC = UNKNOWN;
    /** @cvalue UBLOCK_RUMI_NUMERAL_SYMBOLS */
    public const int BLOCK_CODE_RUMI_NUMERAL_SYMBOLS = UNKNOWN;
    /** @cvalue UBLOCK_KAITHI */
    public const int BLOCK_CODE_KAITHI = UNKNOWN;
    /** @cvalue UBLOCK_EGYPTIAN_HIEROGLYPHS */
    public const int BLOCK_CODE_EGYPTIAN_HIEROGLYPHS = UNKNOWN;
    /** @cvalue UBLOCK_ENCLOSED_ALPHANUMERIC_SUPPLEMENT */
    public const int BLOCK_CODE_ENCLOSED_ALPHANUMERIC_SUPPLEMENT = UNKNOWN;
    /** @cvalue UBLOCK_ENCLOSED_IDEOGRAPHIC_SUPPLEMENT */
    public const int BLOCK_CODE_ENCLOSED_IDEOGRAPHIC_SUPPLEMENT = UNKNOWN;
    /** @cvalue UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_C */
    public const int BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_C = UNKNOWN;
    /** @cvalue UBLOCK_MANDAIC */
    public const int BLOCK_CODE_MANDAIC = UNKNOWN;
    /** @cvalue UBLOCK_BATAK */
    public const int BLOCK_CODE_BATAK = UNKNOWN;
    /** @cvalue UBLOCK_ETHIOPIC_EXTENDED_A */
    public const int BLOCK_CODE_ETHIOPIC_EXTENDED_A = UNKNOWN;
    /** @cvalue UBLOCK_BRAHMI */
    public const int BLOCK_CODE_BRAHMI = UNKNOWN;
    /** @cvalue UBLOCK_BAMUM_SUPPLEMENT */
    public const int BLOCK_CODE_BAMUM_SUPPLEMENT = UNKNOWN;
    /** @cvalue UBLOCK_KANA_SUPPLEMENT */
    public const int BLOCK_CODE_KANA_SUPPLEMENT = UNKNOWN;
    /** @cvalue UBLOCK_PLAYING_CARDS */
    public const int BLOCK_CODE_PLAYING_CARDS = UNKNOWN;
    /** @cvalue UBLOCK_MISCELLANEOUS_SYMBOLS_AND_PICTOGRAPHS */
    public const int BLOCK_CODE_MISCELLANEOUS_SYMBOLS_AND_PICTOGRAPHS = UNKNOWN;
    /** @cvalue UBLOCK_EMOTICONS */
    public const int BLOCK_CODE_EMOTICONS = UNKNOWN;
    /** @cvalue UBLOCK_TRANSPORT_AND_MAP_SYMBOLS */
    public const int BLOCK_CODE_TRANSPORT_AND_MAP_SYMBOLS = UNKNOWN;
    /** @cvalue UBLOCK_ALCHEMICAL_SYMBOLS */
    public const int BLOCK_CODE_ALCHEMICAL_SYMBOLS = UNKNOWN;
    /** @cvalue UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_D */
    public const int BLOCK_CODE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_D = UNKNOWN;
    /** @cvalue UBLOCK_ARABIC_EXTENDED_A */
    public const int BLOCK_CODE_ARABIC_EXTENDED_A = UNKNOWN;
    /** @cvalue UBLOCK_ARABIC_MATHEMATICAL_ALPHABETIC_SYMBOLS */
    public const int BLOCK_CODE_ARABIC_MATHEMATICAL_ALPHABETIC_SYMBOLS = UNKNOWN;
    /** @cvalue UBLOCK_CHAKMA */
    public const int BLOCK_CODE_CHAKMA = UNKNOWN;
    /** @cvalue UBLOCK_MEETEI_MAYEK_EXTENSIONS */
    public const int BLOCK_CODE_MEETEI_MAYEK_EXTENSIONS = UNKNOWN;
    /** @cvalue UBLOCK_MEROITIC_CURSIVE */
    public const int BLOCK_CODE_MEROITIC_CURSIVE = UNKNOWN;
    /** @cvalue UBLOCK_MEROITIC_HIEROGLYPHS */
    public const int BLOCK_CODE_MEROITIC_HIEROGLYPHS = UNKNOWN;
    /** @cvalue UBLOCK_MIAO */
    public const int BLOCK_CODE_MIAO = UNKNOWN;
    /** @cvalue UBLOCK_SHARADA */
    public const int BLOCK_CODE_SHARADA = UNKNOWN;
    /** @cvalue UBLOCK_SORA_SOMPENG */
    public const int BLOCK_CODE_SORA_SOMPENG = UNKNOWN;
    /** @cvalue UBLOCK_SUNDANESE_SUPPLEMENT */
    public const int BLOCK_CODE_SUNDANESE_SUPPLEMENT = UNKNOWN;
    /** @cvalue UBLOCK_TAKRI */
    public const int BLOCK_CODE_TAKRI = UNKNOWN;
#if U_ICU_VERSION_MAJOR_NUM >= 54
    /** @cvalue UBLOCK_BASSA_VAH */
    public const int BLOCK_CODE_BASSA_VAH = UNKNOWN;
    /** @cvalue UBLOCK_CAUCASIAN_ALBANIAN */
    public const int BLOCK_CODE_CAUCASIAN_ALBANIAN = UNKNOWN;
    /** @cvalue UBLOCK_COPTIC_EPACT_NUMBERS */
    public const int BLOCK_CODE_COPTIC_EPACT_NUMBERS = UNKNOWN;
    /** @cvalue UBLOCK_COMBINING_DIACRITICAL_MARKS_EXTENDED */
    public const int BLOCK_CODE_COMBINING_DIACRITICAL_MARKS_EXTENDED = UNKNOWN;
    /** @cvalue UBLOCK_DUPLOYAN */
    public const int BLOCK_CODE_DUPLOYAN = UNKNOWN;
    /** @cvalue UBLOCK_ELBASAN */
    public const int BLOCK_CODE_ELBASAN = UNKNOWN;
    /** @cvalue UBLOCK_GEOMETRIC_SHAPES_EXTENDED */
    public const int BLOCK_CODE_GEOMETRIC_SHAPES_EXTENDED = UNKNOWN;
    /** @cvalue UBLOCK_GRANTHA */
    public const int BLOCK_CODE_GRANTHA = UNKNOWN;
    /** @cvalue UBLOCK_KHOJKI */
    public const int BLOCK_CODE_KHOJKI = UNKNOWN;
    /** @cvalue UBLOCK_KHUDAWADI */
    public const int BLOCK_CODE_KHUDAWADI = UNKNOWN;
    /** @cvalue UBLOCK_LATIN_EXTENDED_E */
    public const int BLOCK_CODE_LATIN_EXTENDED_E = UNKNOWN;
    /** @cvalue UBLOCK_LINEAR_A */
    public const int BLOCK_CODE_LINEAR_A = UNKNOWN;
    /** @cvalue UBLOCK_MAHAJANI */
    public const int BLOCK_CODE_MAHAJANI = UNKNOWN;
    /** @cvalue UBLOCK_MANICHAEAN */
    public const int BLOCK_CODE_MANICHAEAN = UNKNOWN;
    /** @cvalue UBLOCK_MENDE_KIKAKUI */
    public const int BLOCK_CODE_MENDE_KIKAKUI = UNKNOWN;
    /** @cvalue UBLOCK_MODI */
    public const int BLOCK_CODE_MODI = UNKNOWN;
    /** @cvalue UBLOCK_MRO */
    public const int BLOCK_CODE_MRO = UNKNOWN;
    /** @cvalue UBLOCK_MYANMAR_EXTENDED_B */
    public const int BLOCK_CODE_MYANMAR_EXTENDED_B = UNKNOWN;
    /** @cvalue UBLOCK_NABATAEAN */
    public const int BLOCK_CODE_NABATAEAN = UNKNOWN;
    /** @cvalue UBLOCK_OLD_NORTH_ARABIAN */
    public const int BLOCK_CODE_OLD_NORTH_ARABIAN = UNKNOWN;
    /** @cvalue UBLOCK_OLD_PERMIC */
    public const int BLOCK_CODE_OLD_PERMIC = UNKNOWN;
    /** @cvalue UBLOCK_ORNAMENTAL_DINGBATS */
    public const int BLOCK_CODE_ORNAMENTAL_DINGBATS = UNKNOWN;
    /** @cvalue UBLOCK_PAHAWH_HMONG */
    public const int BLOCK_CODE_PAHAWH_HMONG = UNKNOWN;
    /** @cvalue UBLOCK_PALMYRENE */
    public const int BLOCK_CODE_PALMYRENE = UNKNOWN;
    /** @cvalue UBLOCK_PAU_CIN_HAU */
    public const int BLOCK_CODE_PAU_CIN_HAU = UNKNOWN;
    /** @cvalue UBLOCK_PSALTER_PAHLAVI */
    public const int BLOCK_CODE_PSALTER_PAHLAVI = UNKNOWN;
    /** @cvalue UBLOCK_SHORTHAND_FORMAT_CONTROLS */
    public const int BLOCK_CODE_SHORTHAND_FORMAT_CONTROLS = UNKNOWN;
    /** @cvalue UBLOCK_SIDDHAM */
    public const int BLOCK_CODE_SIDDHAM = UNKNOWN;
    /** @cvalue UBLOCK_SINHALA_ARCHAIC_NUMBERS */
    public const int BLOCK_CODE_SINHALA_ARCHAIC_NUMBERS = UNKNOWN;
    /** @cvalue UBLOCK_SUPPLEMENTAL_ARROWS_C */
    public const int BLOCK_CODE_SUPPLEMENTAL_ARROWS_C = UNKNOWN;
    /** @cvalue UBLOCK_TIRHUTA */
    public const int BLOCK_CODE_TIRHUTA = UNKNOWN;
    /** @cvalue UBLOCK_WARANG_CITI */
    public const int BLOCK_CODE_WARANG_CITI = UNKNOWN;
#endif
    /** @cvalue UBLOCK_COUNT */
    public const int BLOCK_CODE_COUNT = UNKNOWN;
    /** @cvalue UBLOCK_INVALID_CODE */
    public const int BLOCK_CODE_INVALID_CODE = UNKNOWN;

    /* UBidiPairedBracketType - http://icu-project.org/apiref/icu4c/uchar_8h.html#af954219aa1df452657ec355221c6703d */

#if U_ICU_VERSION_MAJOR_NUM >= 52
    /** @cvalue U_BPT_NONE */
    public const int BPT_NONE = UNKNOWN;
    /** @cvalue U_BPT_OPEN */
    public const int BPT_OPEN = UNKNOWN;
    /** @cvalue U_BPT_CLOSE */
    public const int BPT_CLOSE = UNKNOWN;
    /** @cvalue U_BPT_COUNT */
    public const int BPT_COUNT = UNKNOWN;
#endif

    /* UEastAsianWidth - http://icu-project.org/apiref/icu4c/uchar_8h.html#a95cc2ca2f9cfd6d0c63eee2c65951333 */

    /** @cvalue U_EA_NEUTRAL */
    public const int EA_NEUTRAL = UNKNOWN;
    /** @cvalue U_EA_AMBIGUOUS */
    public const int EA_AMBIGUOUS = UNKNOWN;
    /** @cvalue U_EA_HALFWIDTH */
    public const int EA_HALFWIDTH = UNKNOWN;
    /** @cvalue U_EA_FULLWIDTH */
    public const int EA_FULLWIDTH = UNKNOWN;
    /** @cvalue U_EA_NARROW */
    public const int EA_NARROW = UNKNOWN;
    /** @cvalue U_EA_WIDE */
    public const int EA_WIDE = UNKNOWN;
    /** @cvalue U_EA_COUNT */
    public const int EA_COUNT = UNKNOWN;

    /* UCharNameChoice - http://icu-project.org/apiref/icu4c/uchar_8h.html#a2ba37edcca62eff48226e8096035addf */

    /** @cvalue U_UNICODE_CHAR_NAME */
    public const int UNICODE_CHAR_NAME = UNKNOWN;
    /** @cvalue U_UNICODE_10_CHAR_NAME */
    public const int UNICODE_10_CHAR_NAME = UNKNOWN;
    /** @cvalue U_EXTENDED_CHAR_NAME */
    public const int EXTENDED_CHAR_NAME = UNKNOWN;
    /** @cvalue U_CHAR_NAME_ALIAS */
    public const int CHAR_NAME_ALIAS = UNKNOWN;
    /** @cvalue U_CHAR_NAME_CHOICE_COUNT */
    public const int CHAR_NAME_CHOICE_COUNT = UNKNOWN;

    /* UPropertyNameChoice - http://icu-project.org/apiref/icu4c/uchar_8h.html#a5056494c7d5a2c7185f3c464f48fe5d1 */

    /** @cvalue U_SHORT_PROPERTY_NAME */
    public const int SHORT_PROPERTY_NAME = UNKNOWN;
    /** @cvalue U_LONG_PROPERTY_NAME */
    public const int LONG_PROPERTY_NAME = UNKNOWN;
    /** @cvalue U_PROPERTY_NAME_CHOICE_COUNT */
    public const int PROPERTY_NAME_CHOICE_COUNT = UNKNOWN;

    /* UDecompositionType - http://icu-project.org/apiref/icu4c/uchar_8h.html#ae2c56994fcf28062c7e77beb671533f5 */

    /** @cvalue U_DT_NONE */
    public const int DT_NONE = UNKNOWN;
    /** @cvalue U_DT_CANONICAL */
    public const int DT_CANONICAL = UNKNOWN;
    /** @cvalue U_DT_COMPAT */
    public const int DT_COMPAT = UNKNOWN;
    /** @cvalue U_DT_CIRCLE */
    public const int DT_CIRCLE = UNKNOWN;
    /** @cvalue U_DT_FINAL */
    public const int DT_FINAL = UNKNOWN;
    /** @cvalue U_DT_FONT */
    public const int DT_FONT = UNKNOWN;
    /** @cvalue U_DT_FRACTION */
    public const int DT_FRACTION = UNKNOWN;
    /** @cvalue U_DT_INITIAL */
    public const int DT_INITIAL = UNKNOWN;
    /** @cvalue U_DT_ISOLATED */
    public const int DT_ISOLATED = UNKNOWN;
    /** @cvalue U_DT_MEDIAL */
    public const int DT_MEDIAL = UNKNOWN;
    /** @cvalue U_DT_NARROW */
    public const int DT_NARROW = UNKNOWN;
    /** @cvalue U_DT_NOBREAK */
    public const int DT_NOBREAK = UNKNOWN;
    /** @cvalue U_DT_SMALL */
    public const int DT_SMALL = UNKNOWN;
    /** @cvalue U_DT_SQUARE */
    public const int DT_SQUARE = UNKNOWN;
    /** @cvalue U_DT_SUB */
    public const int DT_SUB = UNKNOWN;
    /** @cvalue U_DT_SUPER */
    public const int DT_SUPER = UNKNOWN;
    /** @cvalue U_DT_VERTICAL */
    public const int DT_VERTICAL = UNKNOWN;
    /** @cvalue U_DT_WIDE */
    public const int DT_WIDE = UNKNOWN;
    /** @cvalue U_DT_COUNT */
    public const int DT_COUNT = UNKNOWN;

    /* UJoiningType - http://icu-project.org/apiref/icu4c/uchar_8h.html#a3ce1ce20e7f3b8534eb3490ad3aba3dd */

    /** @cvalue U_JT_NON_JOINING */
    public const int JT_NON_JOINING = UNKNOWN;
    /** @cvalue U_JT_JOIN_CAUSING */
    public const int JT_JOIN_CAUSING = UNKNOWN;
    /** @cvalue U_JT_DUAL_JOINING */
    public const int JT_DUAL_JOINING = UNKNOWN;
    /** @cvalue U_JT_LEFT_JOINING */
    public const int JT_LEFT_JOINING = UNKNOWN;
    /** @cvalue U_JT_RIGHT_JOINING */
    public const int JT_RIGHT_JOINING = UNKNOWN;
    /** @cvalue U_JT_TRANSPARENT */
    public const int JT_TRANSPARENT = UNKNOWN;
    /** @cvalue U_JT_COUNT */
    public const int JT_COUNT = UNKNOWN;

    /* UJoiningGroup - http://icu-project.org/apiref/icu4c/uchar_8h.html#a7887844ec0872e6e9a283e0825fcae65 */

    /** @cvalue U_JG_NO_JOINING_GROUP */
    public const int JG_NO_JOINING_GROUP = UNKNOWN;
    /** @cvalue U_JG_AIN */
    public const int JG_AIN = UNKNOWN;
    /** @cvalue U_JG_ALAPH */
    public const int JG_ALAPH = UNKNOWN;
    /** @cvalue U_JG_ALEF */
    public const int JG_ALEF = UNKNOWN;
    /** @cvalue U_JG_BEH */
    public const int JG_BEH = UNKNOWN;
    /** @cvalue U_JG_BETH */
    public const int JG_BETH = UNKNOWN;
    /** @cvalue U_JG_DAL */
    public const int JG_DAL = UNKNOWN;
    /** @cvalue U_JG_DALATH_RISH */
    public const int JG_DALATH_RISH = UNKNOWN;
    /** @cvalue U_JG_E */
    public const int JG_E = UNKNOWN;
    /** @cvalue U_JG_FEH */
    public const int JG_FEH = UNKNOWN;
    /** @cvalue U_JG_FINAL_SEMKATH */
    public const int JG_FINAL_SEMKATH = UNKNOWN;
    /** @cvalue U_JG_GAF */
    public const int JG_GAF = UNKNOWN;
    /** @cvalue U_JG_GAMAL */
    public const int JG_GAMAL = UNKNOWN;
    /** @cvalue U_JG_HAH */
    public const int JG_HAH = UNKNOWN;
    /** @cvalue U_JG_TEH_MARBUTA_GOAL */
    public const int JG_TEH_MARBUTA_GOAL = UNKNOWN;
    /** @cvalue U_JG_HAMZA_ON_HEH_GOAL */
    public const int JG_HAMZA_ON_HEH_GOAL = UNKNOWN;
    /** @cvalue U_JG_HE */
    public const int JG_HE = UNKNOWN;
    /** @cvalue U_JG_HEH */
    public const int JG_HEH = UNKNOWN;
    /** @cvalue U_JG_HEH_GOAL */
    public const int JG_HEH_GOAL = UNKNOWN;
    /** @cvalue U_JG_HETH */
    public const int JG_HETH = UNKNOWN;
    /** @cvalue U_JG_KAF */
    public const int JG_KAF = UNKNOWN;
    /** @cvalue U_JG_KAPH */
    public const int JG_KAPH = UNKNOWN;
    /** @cvalue U_JG_KNOTTED_HEH */
    public const int JG_KNOTTED_HEH = UNKNOWN;
    /** @cvalue U_JG_LAM */
    public const int JG_LAM = UNKNOWN;
    /** @cvalue U_JG_LAMADH */
    public const int JG_LAMADH = UNKNOWN;
    /** @cvalue U_JG_MEEM */
    public const int JG_MEEM = UNKNOWN;
    /** @cvalue U_JG_MIM */
    public const int JG_MIM = UNKNOWN;
    /** @cvalue U_JG_NOON */
    public const int JG_NOON = UNKNOWN;
    /** @cvalue U_JG_NUN */
    public const int JG_NUN = UNKNOWN;
    /** @cvalue U_JG_PE */
    public const int JG_PE = UNKNOWN;
    /** @cvalue U_JG_QAF */
    public const int JG_QAF = UNKNOWN;
    /** @cvalue U_JG_QAPH */
    public const int JG_QAPH = UNKNOWN;
    /** @cvalue U_JG_REH */
    public const int JG_REH = UNKNOWN;
    /** @cvalue U_JG_REVERSED_PE */
    public const int JG_REVERSED_PE = UNKNOWN;
    /** @cvalue U_JG_SAD */
    public const int JG_SAD = UNKNOWN;
    /** @cvalue U_JG_SADHE */
    public const int JG_SADHE = UNKNOWN;
    /** @cvalue U_JG_SEEN */
    public const int JG_SEEN = UNKNOWN;
    /** @cvalue U_JG_SEMKATH */
    public const int JG_SEMKATH = UNKNOWN;
    /** @cvalue U_JG_SHIN */
    public const int JG_SHIN = UNKNOWN;
    /** @cvalue U_JG_SWASH_KAF */
    public const int JG_SWASH_KAF = UNKNOWN;
    /** @cvalue U_JG_SYRIAC_WAW */
    public const int JG_SYRIAC_WAW = UNKNOWN;
    /** @cvalue U_JG_TAH */
    public const int JG_TAH = UNKNOWN;
    /** @cvalue U_JG_TAW */
    public const int JG_TAW = UNKNOWN;
    /** @cvalue U_JG_TEH_MARBUTA */
    public const int JG_TEH_MARBUTA = UNKNOWN;
    /** @cvalue U_JG_TETH */
    public const int JG_TETH = UNKNOWN;
    /** @cvalue U_JG_WAW */
    public const int JG_WAW = UNKNOWN;
    /** @cvalue U_JG_YEH */
    public const int JG_YEH = UNKNOWN;
    /** @cvalue U_JG_YEH_BARREE */
    public const int JG_YEH_BARREE = UNKNOWN;
    /** @cvalue U_JG_YEH_WITH_TAIL */
    public const int JG_YEH_WITH_TAIL = UNKNOWN;
    /** @cvalue U_JG_YUDH */
    public const int JG_YUDH = UNKNOWN;
    /** @cvalue U_JG_YUDH_HE */
    public const int JG_YUDH_HE = UNKNOWN;
    /** @cvalue U_JG_ZAIN */
    public const int JG_ZAIN = UNKNOWN;
    /** @cvalue U_JG_FE */
    public const int JG_FE = UNKNOWN;
    /** @cvalue U_JG_KHAPH */
    public const int JG_KHAPH = UNKNOWN;
    /** @cvalue U_JG_ZHAIN */
    public const int JG_ZHAIN = UNKNOWN;
    /** @cvalue U_JG_BURUSHASKI_YEH_BARREE */
    public const int JG_BURUSHASKI_YEH_BARREE = UNKNOWN;
    /** @cvalue U_JG_FARSI_YEH */
    public const int JG_FARSI_YEH = UNKNOWN;
    /** @cvalue U_JG_NYA */
    public const int JG_NYA = UNKNOWN;
    /** @cvalue U_JG_ROHINGYA_YEH */
    public const int JG_ROHINGYA_YEH = UNKNOWN;
#if U_ICU_VERSION_MAJOR_NUM >= 54
    /** @cvalue U_JG_MANICHAEAN_ALEPH */
    public const int JG_MANICHAEAN_ALEPH = UNKNOWN;
    /** @cvalue U_JG_MANICHAEAN_AYIN */
    public const int JG_MANICHAEAN_AYIN = UNKNOWN;
    /** @cvalue U_JG_MANICHAEAN_BETH */
    public const int JG_MANICHAEAN_BETH = UNKNOWN;
    /** @cvalue U_JG_MANICHAEAN_DALETH */
    public const int JG_MANICHAEAN_DALETH = UNKNOWN;
    /** @cvalue U_JG_MANICHAEAN_DHAMEDH */
    public const int JG_MANICHAEAN_DHAMEDH = UNKNOWN;
    /** @cvalue U_JG_MANICHAEAN_FIVE */
    public const int JG_MANICHAEAN_FIVE = UNKNOWN;
    /** @cvalue U_JG_MANICHAEAN_GIMEL */
    public const int JG_MANICHAEAN_GIMEL = UNKNOWN;
    /** @cvalue U_JG_MANICHAEAN_HETH */
    public const int JG_MANICHAEAN_HETH = UNKNOWN;
    /** @cvalue U_JG_MANICHAEAN_HUNDRED */
    public const int JG_MANICHAEAN_HUNDRED = UNKNOWN;
    /** @cvalue U_JG_MANICHAEAN_KAPH */
    public const int JG_MANICHAEAN_KAPH = UNKNOWN;
    /** @cvalue U_JG_MANICHAEAN_LAMEDH */
    public const int JG_MANICHAEAN_LAMEDH = UNKNOWN;
    /** @cvalue U_JG_MANICHAEAN_MEM */
    public const int JG_MANICHAEAN_MEM = UNKNOWN;
    /** @cvalue U_JG_MANICHAEAN_NUN */
    public const int JG_MANICHAEAN_NUN = UNKNOWN;
    /** @cvalue U_JG_MANICHAEAN_ONE */
    public const int JG_MANICHAEAN_ONE = UNKNOWN;
    /** @cvalue U_JG_MANICHAEAN_PE */
    public const int JG_MANICHAEAN_PE = UNKNOWN;
    /** @cvalue U_JG_MANICHAEAN_QOPH */
    public const int JG_MANICHAEAN_QOPH = UNKNOWN;
    /** @cvalue U_JG_MANICHAEAN_RESH */
    public const int JG_MANICHAEAN_RESH = UNKNOWN;
    /** @cvalue U_JG_MANICHAEAN_SADHE */
    public const int JG_MANICHAEAN_SADHE = UNKNOWN;
    /** @cvalue U_JG_MANICHAEAN_SAMEKH */
    public const int JG_MANICHAEAN_SAMEKH = UNKNOWN;
    /** @cvalue U_JG_MANICHAEAN_TAW */
    public const int JG_MANICHAEAN_TAW = UNKNOWN;
    /** @cvalue U_JG_MANICHAEAN_TEN */
    public const int JG_MANICHAEAN_TEN = UNKNOWN;
    /** @cvalue U_JG_MANICHAEAN_TETH */
    public const int JG_MANICHAEAN_TETH = UNKNOWN;
    /** @cvalue U_JG_MANICHAEAN_THAMEDH */
    public const int JG_MANICHAEAN_THAMEDH = UNKNOWN;
    /** @cvalue U_JG_MANICHAEAN_TWENTY */
    public const int JG_MANICHAEAN_TWENTY = UNKNOWN;
    /** @cvalue U_JG_MANICHAEAN_WAW */
    public const int JG_MANICHAEAN_WAW = UNKNOWN;
    /** @cvalue U_JG_MANICHAEAN_YODH */
    public const int JG_MANICHAEAN_YODH = UNKNOWN;
    /** @cvalue U_JG_MANICHAEAN_ZAYIN */
    public const int JG_MANICHAEAN_ZAYIN = UNKNOWN;
    /** @cvalue U_JG_STRAIGHT_WAW */
    public const int JG_STRAIGHT_WAW = UNKNOWN;
#endif
    /** @cvalue U_JG_COUNT */
    public const int JG_COUNT = UNKNOWN;

    /* UGraphemeClusterBreak - http://icu-project.org/apiref/icu4c/uchar_8h.html#abb9bae7d2a1c80ce342be4647661fde1 */

    /** @cvalue U_GCB_OTHER */
    public const int GCB_OTHER = UNKNOWN;
    /** @cvalue U_GCB_CONTROL */
    public const int GCB_CONTROL = UNKNOWN;
    /** @cvalue U_GCB_CR */
    public const int GCB_CR = UNKNOWN;
    /** @cvalue U_GCB_EXTEND */
    public const int GCB_EXTEND = UNKNOWN;
    /** @cvalue U_GCB_L */
    public const int GCB_L = UNKNOWN;
    /** @cvalue U_GCB_LF */
    public const int GCB_LF = UNKNOWN;
    /** @cvalue U_GCB_LV */
    public const int GCB_LV = UNKNOWN;
    /** @cvalue U_GCB_LVT */
    public const int GCB_LVT = UNKNOWN;
    /** @cvalue U_GCB_T */
    public const int GCB_T = UNKNOWN;
    /** @cvalue U_GCB_V */
    public const int GCB_V = UNKNOWN;
    /** @cvalue U_GCB_SPACING_MARK */
    public const int GCB_SPACING_MARK = UNKNOWN;
    /** @cvalue U_GCB_PREPEND */
    public const int GCB_PREPEND = UNKNOWN;
    /** @cvalue U_GCB_REGIONAL_INDICATOR */
    public const int GCB_REGIONAL_INDICATOR = UNKNOWN;
    /** @cvalue U_GCB_COUNT */
    public const int GCB_COUNT = UNKNOWN;

    /* UWordBreakValues - http://icu-project.org/apiref/icu4c/uchar_8h.html#af70ee907368e663f8dd4b90c7196e15c */

    /** @cvalue U_WB_OTHER */
    public const int WB_OTHER = UNKNOWN;
    /** @cvalue U_WB_ALETTER */
    public const int WB_ALETTER = UNKNOWN;
    /** @cvalue U_WB_FORMAT */
    public const int WB_FORMAT = UNKNOWN;
    /** @cvalue U_WB_KATAKANA */
    public const int WB_KATAKANA = UNKNOWN;
    /** @cvalue U_WB_MIDLETTER */
    public const int WB_MIDLETTER = UNKNOWN;
    /** @cvalue U_WB_MIDNUM */
    public const int WB_MIDNUM = UNKNOWN;
    /** @cvalue U_WB_NUMERIC */
    public const int WB_NUMERIC = UNKNOWN;
    /** @cvalue U_WB_EXTENDNUMLET */
    public const int WB_EXTENDNUMLET = UNKNOWN;
    /** @cvalue U_WB_CR */
    public const int WB_CR = UNKNOWN;
    /** @cvalue U_WB_EXTEND */
    public const int WB_EXTEND = UNKNOWN;
    /** @cvalue U_WB_LF */
    public const int WB_LF = UNKNOWN;
    /** @cvalue U_WB_MIDNUMLET */
    public const int WB_MIDNUMLET = UNKNOWN;
    /** @cvalue U_WB_NEWLINE */
    public const int WB_NEWLINE = UNKNOWN;
    /** @cvalue U_WB_REGIONAL_INDICATOR */
    public const int WB_REGIONAL_INDICATOR = UNKNOWN;
#if U_ICU_VERSION_MAJOR_NUM >= 52
    /** @cvalue U_WB_HEBREW_LETTER */
    public const int WB_HEBREW_LETTER = UNKNOWN;
    /** @cvalue U_WB_SINGLE_QUOTE */
    public const int WB_SINGLE_QUOTE = UNKNOWN;
    /** @cvalue U_WB_DOUBLE_QUOTE */
    public const int WB_DOUBLE_QUOTE = UNKNOWN;
#endif
    /** @cvalue U_WB_COUNT */
    public const int WB_COUNT = UNKNOWN;

    /* USentenceBreak - http://icu-project.org/apiref/icu4c/uchar_8h.html#a89e9e463c3bae1d2d46b1dbb6f90de0f */

    /** @cvalue U_SB_OTHER */
    public const int SB_OTHER = UNKNOWN;
    /** @cvalue U_SB_ATERM */
    public const int SB_ATERM = UNKNOWN;
    /** @cvalue U_SB_CLOSE */
    public const int SB_CLOSE = UNKNOWN;
    /** @cvalue U_SB_FORMAT */
    public const int SB_FORMAT = UNKNOWN;
    /** @cvalue U_SB_LOWER */
    public const int SB_LOWER = UNKNOWN;
    /** @cvalue U_SB_NUMERIC */
    public const int SB_NUMERIC = UNKNOWN;
    /** @cvalue U_SB_OLETTER */
    public const int SB_OLETTER = UNKNOWN;
    /** @cvalue U_SB_SEP */
    public const int SB_SEP = UNKNOWN;
    /** @cvalue U_SB_SP */
    public const int SB_SP = UNKNOWN;
    /** @cvalue U_SB_STERM */
    public const int SB_STERM = UNKNOWN;
    /** @cvalue U_SB_UPPER */
    public const int SB_UPPER = UNKNOWN;
    /** @cvalue U_SB_CR */
    public const int SB_CR = UNKNOWN;
    /** @cvalue U_SB_EXTEND */
    public const int SB_EXTEND = UNKNOWN;
    /** @cvalue U_SB_LF */
    public const int SB_LF = UNKNOWN;
    /** @cvalue U_SB_SCONTINUE */
    public const int SB_SCONTINUE = UNKNOWN;
    /** @cvalue U_SB_COUNT */
    public const int SB_COUNT = UNKNOWN;

    /* ULineBreak - http://icu-project.org/apiref/icu4c/uchar_8h.html#a5d1abdf05be22cb9599f804a8506277c */

    /** @cvalue U_LB_UNKNOWN */
    public const int LB_UNKNOWN = UNKNOWN;
    /** @cvalue U_LB_AMBIGUOUS */
    public const int LB_AMBIGUOUS = UNKNOWN;
    /** @cvalue U_LB_ALPHABETIC */
    public const int LB_ALPHABETIC = UNKNOWN;
    /** @cvalue U_LB_BREAK_BOTH */
    public const int LB_BREAK_BOTH = UNKNOWN;
    /** @cvalue U_LB_BREAK_AFTER */
    public const int LB_BREAK_AFTER = UNKNOWN;
    /** @cvalue U_LB_BREAK_BEFORE */
    public const int LB_BREAK_BEFORE = UNKNOWN;
    /** @cvalue U_LB_MANDATORY_BREAK */
    public const int LB_MANDATORY_BREAK = UNKNOWN;
    /** @cvalue U_LB_CONTINGENT_BREAK */
    public const int LB_CONTINGENT_BREAK = UNKNOWN;
    /** @cvalue U_LB_CLOSE_PUNCTUATION */
    public const int LB_CLOSE_PUNCTUATION = UNKNOWN;
    /** @cvalue U_LB_COMBINING_MARK */
    public const int LB_COMBINING_MARK = UNKNOWN;
    /** @cvalue U_LB_CARRIAGE_RETURN */
    public const int LB_CARRIAGE_RETURN = UNKNOWN;
    /** @cvalue U_LB_EXCLAMATION */
    public const int LB_EXCLAMATION = UNKNOWN;
    /** @cvalue U_LB_GLUE */
    public const int LB_GLUE = UNKNOWN;
    /** @cvalue U_LB_HYPHEN */
    public const int LB_HYPHEN = UNKNOWN;
    /** @cvalue U_LB_IDEOGRAPHIC */
    public const int LB_IDEOGRAPHIC = UNKNOWN;
    /** @cvalue U_LB_INSEPARABLE */
    public const int LB_INSEPARABLE = UNKNOWN;
    /** @cvalue U_LB_INSEPERABLE */
    public const int LB_INSEPERABLE = UNKNOWN;
    /** @cvalue U_LB_INFIX_NUMERIC */
    public const int LB_INFIX_NUMERIC = UNKNOWN;
    /** @cvalue U_LB_LINE_FEED */
    public const int LB_LINE_FEED = UNKNOWN;
    /** @cvalue U_LB_NONSTARTER */
    public const int LB_NONSTARTER = UNKNOWN;
    /** @cvalue U_LB_NUMERIC */
    public const int LB_NUMERIC = UNKNOWN;
    /** @cvalue U_LB_OPEN_PUNCTUATION */
    public const int LB_OPEN_PUNCTUATION = UNKNOWN;
    /** @cvalue U_LB_POSTFIX_NUMERIC */
    public const int LB_POSTFIX_NUMERIC = UNKNOWN;
    /** @cvalue U_LB_PREFIX_NUMERIC */
    public const int LB_PREFIX_NUMERIC = UNKNOWN;
    /** @cvalue U_LB_QUOTATION */
    public const int LB_QUOTATION = UNKNOWN;
    /** @cvalue U_LB_COMPLEX_CONTEXT */
    public const int LB_COMPLEX_CONTEXT = UNKNOWN;
    /** @cvalue U_LB_SURROGATE */
    public const int LB_SURROGATE = UNKNOWN;
    /** @cvalue U_LB_SPACE */
    public const int LB_SPACE = UNKNOWN;
    /** @cvalue U_LB_BREAK_SYMBOLS */
    public const int LB_BREAK_SYMBOLS = UNKNOWN;
    /** @cvalue U_LB_ZWSPACE */
    public const int LB_ZWSPACE = UNKNOWN;
    /** @cvalue U_LB_NEXT_LINE */
    public const int LB_NEXT_LINE = UNKNOWN;
    /** @cvalue U_LB_WORD_JOINER */
    public const int LB_WORD_JOINER = UNKNOWN;
    /** @cvalue U_LB_H2 */
    public const int LB_H2 = UNKNOWN;
    /** @cvalue U_LB_H3 */
    public const int LB_H3 = UNKNOWN;
    /** @cvalue U_LB_JL */
    public const int LB_JL = UNKNOWN;
    /** @cvalue U_LB_JT */
    public const int LB_JT = UNKNOWN;
    /** @cvalue U_LB_JV */
    public const int LB_JV = UNKNOWN;
    /** @cvalue U_LB_CLOSE_PARENTHESIS */
    public const int LB_CLOSE_PARENTHESIS = UNKNOWN;
    /** @cvalue U_LB_CONDITIONAL_JAPANESE_STARTER */
    public const int LB_CONDITIONAL_JAPANESE_STARTER = UNKNOWN;
    /** @cvalue U_LB_HEBREW_LETTER */
    public const int LB_HEBREW_LETTER = UNKNOWN;
    /** @cvalue U_LB_REGIONAL_INDICATOR */
    public const int LB_REGIONAL_INDICATOR = UNKNOWN;
    /** @cvalue U_LB_COUNT */
    public const int LB_COUNT = UNKNOWN;

    /* UNumericType - http://icu-project.org/apiref/icu4c/uchar_8h.html#adec3e7a6ae3a00274c019b3b2ddaecbe */

    /** @cvalue U_NT_NONE */
    public const int NT_NONE = UNKNOWN;
    /** @cvalue U_NT_DECIMAL */
    public const int NT_DECIMAL = UNKNOWN;
    /** @cvalue U_NT_DIGIT */
    public const int NT_DIGIT = UNKNOWN;
    /** @cvalue U_NT_NUMERIC */
    public const int NT_NUMERIC = UNKNOWN;
    /** @cvalue U_NT_COUNT */
    public const int NT_COUNT = UNKNOWN;

    /* UHangulSyllableType - http://icu-project.org/apiref/icu4c/uchar_8h.html#a7cb09027c37ad73571cf541caf002c8f */

    /** @cvalue U_HST_NOT_APPLICABLE */
    public const int HST_NOT_APPLICABLE = UNKNOWN;
    /** @cvalue U_HST_LEADING_JAMO */
    public const int HST_LEADING_JAMO = UNKNOWN;
    /** @cvalue U_HST_VOWEL_JAMO */
    public const int HST_VOWEL_JAMO = UNKNOWN;
    /** @cvalue U_HST_TRAILING_JAMO */
    public const int HST_TRAILING_JAMO = UNKNOWN;
    /** @cvalue U_HST_LV_SYLLABLE */
    public const int HST_LV_SYLLABLE = UNKNOWN;
    /** @cvalue U_HST_LVT_SYLLABLE */
    public const int HST_LVT_SYLLABLE = UNKNOWN;
    /** @cvalue U_HST_COUNT */
    public const int HST_COUNT = UNKNOWN;

    /* StringOptions - http://icu-project.org/apiref/icu4c/stringoptions_8h.html */

    /** @cvalue U_FOLD_CASE_DEFAULT */
    public const int FOLD_CASE_DEFAULT = UNKNOWN;
    /** @cvalue U_FOLD_CASE_EXCLUDE_SPECIAL_I */
    public const int FOLD_CASE_EXCLUDE_SPECIAL_I = UNKNOWN;

    /** @tentative-return-type */
    public static function hasBinaryProperty(int|string $codepoint, int $property): ?bool {}

    /** @tentative-return-type */
    public static function charAge(int|string $codepoint): ?array {}

    /** @tentative-return-type */
    public static function charDigitValue(int|string $codepoint): ?int {}

    /** @tentative-return-type */
    public static function charDirection(int|string $codepoint): ?int {}

    /** @tentative-return-type */
    public static function charFromName(string $name, int $type = IntlChar::UNICODE_CHAR_NAME): ?int {}

    /** @tentative-return-type */
    public static function charMirror(int|string $codepoint): int|string|null {}

    /** @tentative-return-type */
    public static function charName(int|string $codepoint, int $type = IntlChar::UNICODE_CHAR_NAME): ?string {}

    /** @tentative-return-type */
    public static function charType(int|string $codepoint): ?int {}

    /** @tentative-return-type */
    public static function chr(int|string $codepoint): ?string {}

    /** @tentative-return-type */
    public static function digit(int|string $codepoint, int $base = 10): int|false|null {}

    /** @tentative-return-type */
    public static function enumCharNames(int|string $start, int|string $end, callable $callback, int $type = IntlChar::UNICODE_CHAR_NAME): bool {}

    /** @tentative-return-type */
    public static function enumCharTypes(callable $callback): void {}

    /** @tentative-return-type */
    public static function foldCase(int|string $codepoint, int $options = IntlChar::FOLD_CASE_DEFAULT): int|string|null {}

    /** @tentative-return-type */
    public static function forDigit(int $digit, int $base = 10): int {}

#if U_ICU_VERSION_MAJOR_NUM >= 52
    /** @tentative-return-type */
    public static function getBidiPairedBracket(int|string $codepoint): int|string|null {}
#endif

    /** @tentative-return-type */
    public static function getBlockCode(int|string $codepoint): ?int {}

    /** @tentative-return-type */
    public static function getCombiningClass(int|string $codepoint): ?int {}

    /** @tentative-return-type */
    public static function getFC_NFKC_Closure(int|string $codepoint): string|false|null {}

    /** @tentative-return-type */
    public static function getIntPropertyMaxValue(int $property): int {}

    /** @tentative-return-type */
    public static function getIntPropertyMinValue(int $property): int {}

    /** @tentative-return-type */
    public static function getIntPropertyValue(int|string $codepoint, int $property): ?int {}

    /** @tentative-return-type */
    public static function getNumericValue(int|string $codepoint): ?float {}

    /** @tentative-return-type */
    public static function getPropertyEnum(string $alias): int {}

    /** @tentative-return-type */
    public static function getPropertyName(int $property, int $type = IntlChar::LONG_PROPERTY_NAME): string|false {}

    /** @tentative-return-type */
    public static function getPropertyValueEnum(int $property, string $name): int {}

    /** @tentative-return-type */
    public static function getPropertyValueName(int $property, int $value, int $type = IntlChar::LONG_PROPERTY_NAME): string|false {}

    /** @tentative-return-type */
    public static function getUnicodeVersion(): array {}

    /** @tentative-return-type */
    public static function isalnum(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isalpha(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isbase(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isblank(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function iscntrl(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isdefined(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isdigit(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isgraph(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isIDIgnorable(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isIDPart(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isIDStart(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isISOControl(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isJavaIDPart(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isJavaIDStart(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isJavaSpaceChar(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function islower(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isMirrored(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isprint(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function ispunct(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isspace(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function istitle(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isUAlphabetic(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isULowercase(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isupper(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isUUppercase(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isUWhiteSpace(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isWhitespace(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function isxdigit(int|string $codepoint): ?bool {}

    /** @tentative-return-type */
    public static function ord(int|string $character): ?int {}

    /** @tentative-return-type */
    public static function tolower(int|string $codepoint): int|string|null {}

    /** @tentative-return-type */
    public static function totitle(int|string $codepoint): int|string|null {}

    /** @tentative-return-type */
    public static function toupper(int|string $codepoint): int|string|null {}
}
