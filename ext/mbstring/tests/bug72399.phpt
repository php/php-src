--TEST--
Bug #72399 (Use-After-Free in MBString (search_re))
--SKIPIF--
<?php
require 'skipif.inc';
if (!function_exists('mb_ereg')) die('skip mbregex support not available');
?>
--FILE--
<?php
$var5 = mb_ereg_search_init("","2");
$var6 = mb_eregi_replace("2","","");

try {
    $var13 = mb_ereg_search_pos();
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
No regex given
