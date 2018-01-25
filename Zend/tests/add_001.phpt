--TEST--
adding arrays
--FILE--
<?php

$a = array(1,2,3);
$b = array("str", "here");

$c = $a + $b;
var_dump($c);

$a = array(1,2,3);
$b = array(1,2,4);

$c = $a + $b;
var_dump($c);

$a = array("a"=>"aaa",2,3);
$b = array(1,2,"a"=>"bbbbbb");

$c = $a + $b;
var_dump($c);

$a += $b;
var_dump($c);

$a += $a;
var_dump($c);

echo "Done\n";
?>
--EXPECTF--
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(3) {
  ["a"]=>
  string(3) "aaa"
  [0]=>
  int(2)
  [1]=>
  int(3)
}
array(3) {
  ["a"]=>
  string(3) "aaa"
  [0]=>
  int(2)
  [1]=>
  int(3)
}
array(3) {
  ["a"]=>
  string(3) "aaa"
  [0]=>
  int(2)
  [1]=>
  int(3)
}
Done
