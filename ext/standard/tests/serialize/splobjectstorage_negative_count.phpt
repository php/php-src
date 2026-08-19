--TEST--
OSS-Fuzz: Unserializing SplObjectStorage with negative number of elements
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip requires 64-bit"); ?>
--FILE--
<?php

$str = 'C:16:"SplObjectStorage":25:{x:i:-9223372036854775808;}';
try {
    var_dump(unserialize($str));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
UnexpectedValueException: Error at offset 24 of 25 bytes
