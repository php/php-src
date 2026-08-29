--TEST--
fseek(): $whence values that overflow int must return -1, not alias onto a valid constant
--SKIPIF--
<?php
if (PHP_INT_SIZE < 8) die("skip 64-bit only");
?>
--FILE--
<?php
$tmp = __DIR__ . '/fseek_whence_overflow.tmp';
file_put_contents($tmp, "0123456789");
$bias = 2 ** 32;

$h = fopen($tmp, 'r');

// SEEK_CUR + 2**32 must not alias onto SEEK_CUR (1)
fseek($h, 4);
var_dump(fseek($h, 3, SEEK_CUR + $bias));  // -1
var_dump(ftell($h));                        // 4 (unchanged)

// SEEK_END + 2**32 must not alias onto SEEK_END (2)
fseek($h, 4);
var_dump(fseek($h, 3, SEEK_END + $bias));  // -1
var_dump(ftell($h));                        // 4 (unchanged)

// PHP_INT_MIN must not alias onto SEEK_SET (0)
fseek($h, 4);
var_dump(fseek($h, 3, PHP_INT_MIN));       // -1
var_dump(ftell($h));                        // 4 (unchanged)

// Sanity: normal SEEK_CUR still works
fseek($h, 4);
var_dump(fseek($h, 3, SEEK_CUR));          // 0
var_dump(ftell($h));                        // 7

fclose($h);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/fseek_whence_overflow.tmp');
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
