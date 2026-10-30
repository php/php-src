--TEST--
OOM array_fill() mixed > HT_MAX_SIZE
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

var_dump(array_fill(-1, 1024*1024*1024+1, (string)time()));

?>
--EXPECTF--
Fatal error: Possible integer overflow in memory allocation (1073741825 * 32 + 32) in %s on line 3
