--TEST--
str_getcsv() doesn't reach hard limit
--INI--
memory_limit=8M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--FILE--
<?php
// Each field costs a zval plus a zend_string.
var_dump(str_getcsv(str_repeat('a,', 512*1024)));
?>
--EXPECTF--
Deprecated: str_getcsv(): the $escape parameter must be provided as its default value will change in %s on line %d

Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
