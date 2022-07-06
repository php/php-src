--TEST--
Create a SplFixedArray from an array using the fromArray() function don't try to preserve the indexes.
--CREDITS--
Philip Norton philipnorton42@gmail.com
--FILE--
<?php
$array = SplFixedArray::fromArray(array(1 => 1,
                                        2 => '2',
                                        3 => false),
                                    false);
var_dump($array);
?>
--EXPECT--
object(SplFixedArray)#1 (3) {
  [0]=>
  int(1)
  [1]=>
  string(1) "2"
  [2]=>
  bool(false)
}
