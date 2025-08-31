--TEST--
Test oniguruma stack limit
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists('mb_ereg')) die('skip mb_ereg not available');
if (version_compare(MB_ONIGURUMA_VERSION, '6.9.3') < 0) {
    die('skip requires Oniguruma 6.9.3');
}
?>
--FILE--
<?php
$s = str_repeat(' ', 30000);

ini_set('mbstring.regex_stack_limit', 10000);
var_dump(mb_ereg('\\s+$', $s));

ini_set('mbstring.regex_stack_limit', 30000);
var_dump(mb_ereg('\\s+$', $s));

ini_set('mbstring.regex_stack_limit', 30001);
var_dump(mb_ereg('\\s+$', $s));

echo 'OK';
?>
--EXPECT--
bool(false)
bool(false)
bool(true)
OK
