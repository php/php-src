--TEST--
grapheme_str_split() doesn't reach hard limit
--EXTENSIONS--
intl
--INI--
memory_limit=8M
--FILE--
<?php
/* Every grapheme becomes a separate string plus an array slot */
grapheme_str_split(str_repeat("a", 400 * 1024));

echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
