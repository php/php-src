--TEST--
grapheme_stristr() function locale dependency test
--EXTENSIONS--
intl
--FILE--
<?php
var_dump(grapheme_stristr("abc", "ａｂｃ", 0));
var_dump(grapheme_stristr("i", "\u{0130}", 0, "tr_TR"));
var_dump(grapheme_stristr("i", "\u{0130}", 0, "en_US"));

echo "=== Invalid locales ===\n";
var_dump(grapheme_stristr("abc", "ａｂｃ", 0, locale: "defaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
var_dump(intl_get_error_code());
var_dump(intl_get_error_message());
?>
--EXPECTF--
string(3) "abc"
string(1) "i"
bool(false)
=== Invalid locales ===
bool(false)
int(%d)
string(74) "grapheme_stristr(): Error creating search object: U_ILLEGAL_ARGUMENT_ERROR"
