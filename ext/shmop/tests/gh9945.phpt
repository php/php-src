--TEST--
GH-9945: shmop_open() must reject keys outside the key_t range
--EXTENSIONS--
shmop
--SKIPIF--
<?php
if (PHP_INT_SIZE !== 8) die('skip only for 64-bit');
if (PHP_OS_FAMILY !== 'Linux' && PHP_OS_FAMILY !== 'Windows') die('skip only for platforms with 32-bit key_t');
?>
--FILE--
<?php
try {
    shmop_open(0x100000000, '', 0644, 1);
} catch (ValueError $exception) {
    echo $exception->getMessage(), "\n";
}
?>
--EXPECT--
shmop_open(): Argument #1 ($key) is out of range
