--TEST--
Bug #72399 (Use-After-Free in MBString (search_re))
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring extension not available');
if (!function_exists('mb_ereg_replace')) die('skip mb_ereg_replace() not available');
?>
--FILE--
<?php
$var5 = mbereg_search_init("","2");
$var6 = mb_eregi_replace("2","","");
$var13 = mbereg_search_pos();
?>
--EXPECTF--
Warning: mbereg_search_pos(): No regex given in %sbug72399.php on line %d
