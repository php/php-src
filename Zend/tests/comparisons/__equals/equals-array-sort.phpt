--TEST--
__equals: Not called by array functions with ordering semantics (sorting)
--FILE--
<?php
include("Equatable.inc");

/** 
 * The $decoy values in Equatable will reverse the ordering here,
 * because there is no __compareTo and __equals won't be called.
 */
$a = new Equatable(1); 
$b = new Equatable(2);
$c = new Equatable(3);

$array = [$a, $b, $c];

sort($array);

var_dump($array[0]);
var_dump($array[1]);
var_dump($array[2]);

?>
--EXPECT--
object(Equatable)#3 (2) {
  ["decoy":protected]=>
  int(-3)
  ["value":protected]=>
  int(3)
}
object(Equatable)#2 (2) {
  ["decoy":protected]=>
  int(-2)
  ["value":protected]=>
  int(2)
}
object(Equatable)#1 (2) {
  ["decoy":protected]=>
  int(-1)
  ["value":protected]=>
  int(1)
}
