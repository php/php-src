--TEST--
GH-9945: sysvmsg must reject keys outside the key_t range
--EXTENSIONS--
sysvmsg
--SKIPIF--
<?php
if (PHP_INT_SIZE !== 8) die('skip only for 64-bit');
if (PHP_OS_FAMILY !== 'Linux') die('skip only for platforms with 32-bit key_t');
?>
--FILE--
<?php
try {
    msg_queue_exists(0x100000000);
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    msg_get_queue(0x100000000);
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: msg_queue_exists(): Argument #1 ($key) is out of range
ValueError: msg_get_queue(): Argument #1 ($key) is out of range
