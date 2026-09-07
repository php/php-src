--TEST--
GH-9945: sem_get() must reject keys outside the key_t range
--EXTENSIONS--
sysvsem
--SKIPIF--
<?php
if (PHP_INT_SIZE !== 8) die('skip only for 64-bit');
if (PHP_OS_FAMILY !== 'Linux') die('skip only for platforms with 32-bit key_t');
?>
--FILE--
<?php
try {
    sem_get(0x100000000);
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: sem_get(): Argument #1 ($key) is out of range
