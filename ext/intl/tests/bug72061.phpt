--TEST--
Bug #72061: Out-of-bounds reads in zif_grapheme_stripos with negative offset
--EXTENSIONS--
intl
--FILE--
<?php

var_dump(grapheme_stripos(str_repeat("ABCD", 16384), "A", -201));
var_dump(grapheme_strpos(str_repeat("ABCD", 16384), "A", -201));
?>
DONE
--EXPECT--
int(65336)
int(65336)
DONE
