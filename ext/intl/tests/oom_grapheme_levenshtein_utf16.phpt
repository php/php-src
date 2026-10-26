--TEST--
grapheme_levenshtein() doesn't reach hard limit - UTF-16 conversion
--EXTENSIONS--
intl
--INI--
memory_limit=8M
--FILE--
<?php
$size = (int) (2.5 * 1024 * 1024);
grapheme_levenshtein(str_repeat("a", $size), str_repeat("b", $size), 1, 2, 3);

echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
