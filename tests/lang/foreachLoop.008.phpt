--TEST--
Foreach loop tests - error case: reference to constant array, with key.
--FILE--
<?php
foreach (array(1,2) as $k=>&$v) {
  var_dump($v);
}
?>
--EXPECTF--
Fatal error: Cannot create references to elements of a temporary array expression in %s on line 2
