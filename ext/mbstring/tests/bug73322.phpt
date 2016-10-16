--TEST--
Bug #73322 (mb_convert_variables detects recursive reference incorrectly)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
$x = ["a", "b"];
mb_convert_variables("UTF-8", "SJIS,EUC-JP", $x);
echo "Done";
?>
--EXPECT--
Done


