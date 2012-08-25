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
Fatal error: Cannot create references to elements of a temporary array expression in %sforeachLoop.007.php on line %d
