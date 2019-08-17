--TEST--
Bug #73646 (mb_ereg_search_init null pointer dereference)
--SKIPIF--
<?php
require 'skipif.inc';
if (!function_exists('mb_ereg')) die('skip mbregex support not available');
?>
--FILE--
<?php
$v1;

try {
    $v1=str_repeat("#", -1);
} catch (\ErrorException $e) {
    echo $e->getMessage() . "\n";
}

var_dump(mb_ereg_search_init($v1));
?>
--EXPECT--
Second argument has to be greater than or equal to 0
bool(true)
