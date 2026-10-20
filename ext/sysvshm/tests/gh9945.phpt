--TEST--
GH-9945: shm_attach() must reject keys outside the key_t range
--EXTENSIONS--
sysvshm
--SKIPIF--
<?php
if (PHP_INT_SIZE !== 8) die('skip only for 64-bit');
if (PHP_OS_FAMILY !== 'Linux' && PHP_OS_FAMILY !== 'Windows') die('skip only for platforms with 32-bit key_t');
?>
--FILE--
<?php
try {
    shm_attach(0x100000000, 0);
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: shm_attach(): Argument #1 ($key) is out of range
