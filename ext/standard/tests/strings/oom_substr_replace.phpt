--TEST--
substr_replace() doesn't reach hard limit
--INI--
memory_limit=8M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--FILE--
<?php
// The replacement is repeated once per array element.
var_dump(substr_replace(array_fill(0, 16, 'x'), str_repeat('y', 2*1024*1024), 0, 1));
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
