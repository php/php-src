--TEST--
SplFileObject::fseek(): $whence values that overflow int must return -1, not alias onto a valid constant
--SKIPIF--
<?php
if (PHP_INT_SIZE < 8) die("skip 64-bit only");
?>
--FILE--
<?php
$tmp = __DIR__ . '/SplFileObject_fseek_whence_overflow.tmp';
file_put_contents($tmp, "0123456789");
$bias = 2 ** 32;

$file = new SplFileObject($tmp);

// SEEK_CUR + 2**32 must not alias onto SEEK_CUR (1)
$file->fseek(4);
var_dump($file->fseek(3, SEEK_CUR + $bias));  // -1
var_dump($file->ftell());                     // 4 (unchanged)

// SEEK_END + 2**32 must not alias onto SEEK_END (2)
$file->fseek(4);
var_dump($file->fseek(3, SEEK_END + $bias));  // -1
var_dump($file->ftell());                     // 4 (unchanged)

// PHP_INT_MIN must not alias onto SEEK_SET (0)
$file->fseek(4);
var_dump($file->fseek(3, PHP_INT_MIN));       // -1
var_dump($file->ftell());                     // 4 (unchanged)

// Sanity: normal SEEK_CUR still works
$file->fseek(4);
var_dump($file->fseek(3, SEEK_CUR));          // 0
var_dump($file->ftell());                     // 7

unset($file);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/SplFileObject_fseek_whence_overflow.tmp');
?>
--EXPECT--
int(-1)
int(4)
int(-1)
int(4)
int(-1)
int(4)
int(0)
int(7)
