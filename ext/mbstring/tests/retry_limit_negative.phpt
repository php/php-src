--TEST--
Oniguruma retry limit is not set from a negative mbstring.regex_retry_limit
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists('mb_ereg')) die('skip mb_ereg not available');
if (@version_compare(MB_ONIGURUMA_VERSION, '6.9.3') < 0) {
    die('skip requires Oniguruma >= 6.9.3');
}
?>
--INI--
error_reporting=E_ALL & ~E_DEPRECATED
mbstring.regex_retry_limit=-1
--FILE--
<?php
/* A negative limit must leave the default of Oniguruma in place, rather than
 * being converted to an unsigned int of UINT_MAX. This match needs more
 * retries than the default allows for. */
$regex = 'A(B|C+)+D|AC+X';
$str = 'A' . str_repeat('C', 24) . 'X';
var_dump(mb_ereg($regex, $str));
?>
--EXPECT--
bool(false)
