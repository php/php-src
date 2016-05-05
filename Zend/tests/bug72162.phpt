--TEST--
Bug #72162 (use-after-free - error_reporting)
--FILE--
<?php
error_reporting(1);
$var11 = new StdClass();
$var16 = error_reporting($var11);
?>
okey
--EXPECT--
okey
