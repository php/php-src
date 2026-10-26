--TEST--
implode() doesn't reach hard limit
--INI--
memory_limit=8M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--FILE--
<?php
// The glue is repeated once per element.
var_dump(implode(str_repeat('-', 1024*1024), array_fill(0, 32, 'x')));
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
