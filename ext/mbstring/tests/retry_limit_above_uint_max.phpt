--TEST--
Oniguruma retry limit beyond the range of an unsigned int
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists('mb_ereg')) die('skip mb_ereg not available');
if (@version_compare(MB_ONIGURUMA_VERSION, '6.9.3') < 0) {
    die('skip requires Oniguruma >= 6.9.3');
}
if (PHP_INT_SIZE <= 4) die('skip a limit beyond an unsigned int needs a wider PHP integer');
if (PHP_OS_FAMILY === 'Windows') die('skip an unsigned long is 4 bytes wide on Windows');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow test');
?>
--INI--
mbstring.regex_retry_limit=4294967296
--FILE--
<?php
/* The limit above is UINT_MAX + 1. It is passed on as an unsigned long, so it
 * has to be honoured rather than dropped for being out of the range of an
 * unsigned int. This match needs more retries than the default allows for. */
$regex = 'A(B|C+)+D|AC+X';
$str = 'A' . str_repeat('C', 24) . 'X';
var_dump(mb_ereg($regex, $str));
?>
--EXPECTF--
Deprecated: Function mb_ereg() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)
