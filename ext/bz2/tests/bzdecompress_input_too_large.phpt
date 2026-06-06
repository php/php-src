--TEST--
bzdecompress() rejects input larger than 4294967296
--EXTENSIONS--
bz2
--INI--
memory_limit=8G
--SKIPIF--
<?php
if (PHP_INT_SIZE < 8) {
    die('skip requires 64-bit PHP');
}
?>
--FILE--
<?php

try {
    $data = str_repeat("A", 4294967296);
    bzdecompress($data);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
bzdecompress(): Argument #1 ($data) must not exceed %d bytes