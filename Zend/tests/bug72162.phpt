--TEST--
Bug #72162 (use-after-free - error_reporting)
--FILE--
<?php
error_reporting(E_ALL);
$var11 = new StdClass();
$var16 = error_reporting($var11);
?>
--EXPECTF--
Catchable fatal error: Object of class stdClass could not be converted to string in %sbug72162.php on line %d
