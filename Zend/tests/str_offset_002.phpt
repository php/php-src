--TEST--
string offset 002
--FILE--
<?php
$a = "aaa";
$x = array(&$a[1]);
?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot indirectly modify string offset in %s:%d
Stack trace:
#0 {main}
  thrown in %sstr_offset_002.php on line 3
