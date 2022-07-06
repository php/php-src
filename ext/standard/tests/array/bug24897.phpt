--TEST--
Bug #24897 (inconsistent behaviour or shuffle() & array_multisort())
--FILE--
<?php
$a = array(1 => 2);
shuffle($a);
var_dump($a);

$a = array(1 => 2);
array_multisort($a);
var_dump($a);
?>
--EXPECT--
array(1) {
  [0]=>
  int(2)
}
array(1) {
  [0]=>
  int(2)
}
