--TEST--
grapheme_str_pad() error conditions
--EXTENSIONS--
intl
--FILE--
<?php
try {
    grapheme_str_pad("abc", 5, "");
} catch (ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    grapheme_str_pad("abc", 5, "-", 3);
} catch (ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}

// No error when there is nothing to pad, like mb_str_pad(): the padding string and the pad type are not checked
var_dump(grapheme_str_pad("abc", 2, ""));
var_dump(grapheme_str_pad("abc", 2, "\xff"));
var_dump(grapheme_str_pad("abc", 3, "-", 3));

// Invalid UTF-8 in the input or in the padding string
var_dump(grapheme_str_pad("a\xffb", 5, "."));
var_dump(intl_get_error_message());
var_dump(grapheme_str_pad("ab\u{00FC}", 5, "\xff"));
?>
--EXPECT--
grapheme_str_pad(): Argument #3 ($pad_string) must not be empty
grapheme_str_pad(): Argument #4 ($pad_type) must be STR_PAD_LEFT, STR_PAD_RIGHT, or STR_PAD_BOTH
string(3) "abc"
string(3) "abc"
string(3) "abc"
bool(false)
string(62) "grapheme_str_pad(): Invalid UTF-8 string: U_INVALID_CHAR_FOUND"
bool(false)
