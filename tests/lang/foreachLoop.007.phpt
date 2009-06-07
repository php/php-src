--TEST--
Foreach loop tests - error case: reference to constant array.
--FILE--
<?php
echo "\nReference to constant array\n";
foreach (array(1,2) as &$v) {
  var_dump($v);
}
?>
--EXPECTF--
Parse error: %s on line 3
