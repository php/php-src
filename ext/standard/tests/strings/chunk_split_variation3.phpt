--TEST--
Test chunk_split() function : usage variations - OOM with chunk length larger than str
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip ZMM is disabled");
?>
--INI--
memory_limit=128M
--FILE--
<?php

$str = str_repeat('a', 100*1024*1024);

var_dump(chunk_split($str, 1024*1024*1024, $str));

?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted%s(tried to allocate %d bytes) in %s on line 5
