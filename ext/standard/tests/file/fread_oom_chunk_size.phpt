--TEST--
fread() with a large chunk size doesn't reach hard limit
--INI--
memory_limit=8M
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--FILE--
<?php
$fp = fopen(__FILE__, 'rb');
stream_set_chunk_size($fp, 100 * 1024 * 1024);
var_dump(fread($fp, 1));
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
