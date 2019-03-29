--TEST--
Bug #72164 (Null Pointer Dereference - mb_ereg_replace)
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring extension not available');
if (!function_exists('mb_ereg_replace')) die('skip mb_ereg_replace() not available');
?>
--FILE--
<?php
$var0 = "e";
$var2 = "";
$var3 = NULL;
$var8 = mbereg_replace($var2,$var3,$var3,$var0);
var_dump($var8);
?>
--EXPECTF--
Deprecated: mbereg_replace(): The 'e' option is deprecated, use mb_ereg_replace_callback instead in %s%ebug72164.php on line %d
string(0) ""
