--TEST--
Bug #72164 (Null Pointer Dereference - mb_ereg_replace)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
$var0 = "e";
$var2 = "";
$var3 = NULL;
$var8 = mbereg_replace($var2,$var3,$var3,$var0);
var_dump($var8);
?>
--EXPECT--
string(0) ""
