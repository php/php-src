--TEST--
nl2br() doesn't reach hard limit
--INI--
memory_limit=8M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--FILE--
<?php
// Every newline expands to six bytes.
var_dump(nl2br(str_repeat("\n", 2 * 1024 * 1024)));
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
