--TEST--
Using array unpacking in an array literal that also has string keys (OSS-Fuzz #17965)
--FILE--
<?php
$y = [1, 2, 3];
$z = "bar";
$x = [...$y, "foo" => $z];
var_dump($x);
?>
--EXPECT--
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  ["foo"]=>
  string(3) "bar"
}
