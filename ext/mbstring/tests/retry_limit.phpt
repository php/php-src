--TEST--
Oniguruma retry limit
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip mbstring not available');
if (!function_exists('mb_ereg')) die('skip mb_ereg not available');
if (version_compare(MB_ONIGURUMA_VERSION, '6.9.3') < 0) {
    die('skip requires Oniguruma >= 6.9.3');
}
?>
--FILE--
<?php

$regex = 'A(B|C+)+D|AC+X';
$str = 'ACCCCCCCCCCCCCCCCCCCX';
var_dump(mb_ereg($regex, $str));
ini_set('mbstring.regex_retry_limit', '100000');
var_dump(mb_ereg($regex, $str));

?>
--EXPECT--
int(1)
bool(false)
