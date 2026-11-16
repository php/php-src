--TEST--
setcookie() doesn't reach hard limit
--INI--
memory_limit=8M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--FILE--
<?php
// The value is URL encoded to three times its size, then copied into the header.
var_dump(setcookie("c", str_repeat("\xff", 2 * 1024 * 1024)));
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
