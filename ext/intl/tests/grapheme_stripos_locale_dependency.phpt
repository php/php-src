--TEST--
grapheme_stripos() function locale dependency test
--EXTENSIONS--
intl
--FILE--
<?php
var_dump(grapheme_stripos("abc", "ａｂｃ", 0));
var_dump(grapheme_stripos("i", "\u{0130}", 0, "tr_TR"));
var_dump(grapheme_stripos("i", "\u{0130}", 0, "en_US"));
?>
--EXPECT--
int(0)
int(0)
bool(false)
