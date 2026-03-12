--TEST--
grapheme_stripos() function locale dependency test
--EXTENSIONS--
intl
--FILE--
<?php
var_dump(grapheme_stripos("abc", "ａｂｃ", 0));
var_dump(grapheme_stripos("i", "\u{0130}", 0, "tr_TR"));
var_dump(grapheme_stripos("i", "\u{0130}", 0, "en_US"));

echo "=== Invalid locales ===\n";
var_dump(grapheme_stripos("abc", "ａｂｃ", 0, locale: "defaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
var_dump(intl_get_error_code());
var_dump(intl_get_error_message());
?>
--EXPECTF--
int(0)
int(0)
bool(false)
=== Invalid locales ===
bool(false)
int(%d)
string(74) "grapheme_stripos(): Error creating search object: U_ILLEGAL_ARGUMENT_ERROR"
