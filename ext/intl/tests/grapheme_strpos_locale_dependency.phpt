--TEST--
grapheme_strpos() function locale dependency test
--EXTENSIONS--
intl
--FILE--
<?php
var_dump(grapheme_strpos("ß", "ss", 0, locale: "en_US"));
var_dump(grapheme_strpos("ß", "ss", 0, locale: "de_DE-u-ks-level1"));
?>
--EXPECT--
bool(false)
int(0)
