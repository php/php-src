--TEST--
__compareTo: Called by array functions with ordering semantics (sorting)
--FILE--
<?php
include("Comparable.inc");

$a = new Comparable(3);
$b = new Comparable(1);
$c = new Comparable(2);

$array = [$a, $b, $c];

sort($array);

var_dump($array[0]);
var_dump($array[1]);
var_dump($array[2]);

?>
--EXPECT--
object(Comparable)#2 (2) {
  ["decoy":protected]=>
  int(-1)
  ["value":protected]=>
  int(1)
}
object(Comparable)#3 (2) {
  ["decoy":protected]=>
  int(-2)
  ["value":protected]=>
  int(2)
}
object(Comparable)#1 (2) {
  ["decoy":protected]=>
  int(-3)
  ["value":protected]=>
  int(3)
}
