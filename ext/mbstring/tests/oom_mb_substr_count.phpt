--TEST--
mb_substr_count() doesn't reach hard limit
--EXTENSIONS--
mbstring
--INI--
memory_limit=8M
--FILE--
<?php
/* The needle is converted to UTF-8 before counting. 0x80 is U+20AC in CP1252,
 * which takes 3 bytes in UTF-8, so a 2 MB needle (memory_limit / 4) grows into
 * a 6 MB buffer. */
mb_substr_count("abc", str_repeat("\x80", 2 * 1024 * 1024), "CP1252");

echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
