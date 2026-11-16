--TEST--
explode() doesn't reach hard limit
--INI--
memory_limit=8M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--FILE--
<?php
// Each piece costs a zval plus a zend_string, so the result is many times
// the size of the input.
$s = str_repeat('a,', 512*1024);
var_dump(explode(',', $s));
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
