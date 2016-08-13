--TEST--
Bug #70182 ($string[] assignment with +=)
--FILE--
<?php

$str = "abc";
$str[] += $str;

?>
--EXPECTF--
Fatal error: Uncaught Error: [] operator not supported for strings in %sbug70182.php:%d
Stack trace:
#0 {main}
  thrown in %sbug70182.php on line %d
