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
$var13 = mb_ereg_search_pos();
?>
--EXPECTF--
Warning: mb_ereg_search_pos(): No regex given in %sbug72399.php on line %d
