--TEST--
levenshtein() doesn't reach hard limit
--INI--
memory_limit=8M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--FILE--
<?php
// Each workspace row is eight times the length of the second string.
var_dump(levenshtein('a', str_repeat('b', 8*1024*1024)));
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
