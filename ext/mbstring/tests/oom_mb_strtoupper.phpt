--TEST--
mb_strtoupper() doesn't reach hard limit
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--INI--
memory_limit=8M
--FILE--
<?php
// 2 MB == memory_limit / 4
mb_strtoupper(str_repeat("\u{0390}", 1024 * 1024), "UTF-8");
echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
