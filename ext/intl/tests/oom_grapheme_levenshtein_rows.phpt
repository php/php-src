--TEST--
grapheme_levenshtein() doesn't reach hard limit - cost rows
--EXTENSIONS--
intl
--INI--
memory_limit=8M
--FILE--
<?php
grapheme_levenshtein("a", str_repeat("b", 1024 * 1024), 1, 2, 3);

echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
