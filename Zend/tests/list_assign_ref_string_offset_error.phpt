--TEST--
String offset error during list() by-ref assignment
--FILE--
<?php

$a = [0];
$v = 'b';
$i = 0;
list(&$a[$i++]) = $v;

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot create references to/from string offsets in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
