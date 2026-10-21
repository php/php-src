--TEST--
OOM array_fill() mixed
--INI--
memory_limit=100M
--SKIPIF--
<?php
if (PHP_INT_SIZE !== 8) {
    die('skip 64-bit only');
}
?>
--FILE--
<?php

var_dump(array_fill(-1, 1024*1024*1024-1, (string)time()));

?>
--EXPECTF--
Fatal error: Allowed memory size of 104857600 bytes exhausted %S(tried to allocate %d bytes) in %s on line 3
