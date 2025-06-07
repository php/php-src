--TEST--
grapheme_stristr() function locale dependency test
--EXTENSIONS--
intl
--FILE--
<?php
var_dump(grapheme_stristr("abc", "ａｂｃ", 0));
var_dump(grapheme_stristr("i", "\u{0130}", 0, "tr_TR"));
var_dump(grapheme_stristr("i", "\u{0130}", 0, "en_US"));
?>
--EXPECT--
string(3) "abc"
string(1) "i"
bool(false)

