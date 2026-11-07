--TEST--
grapheme_str_pad() respects memory_limit when the result is too large
--EXTENSIONS--
intl
--INI--
memory_limit=128M
--FILE--
<?php

var_dump(grapheme_str_pad("a", PHP_INT_MAX, "b"));

?>
--EXPECTF--
Fatal error: Allowed memory size of 134217728 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
