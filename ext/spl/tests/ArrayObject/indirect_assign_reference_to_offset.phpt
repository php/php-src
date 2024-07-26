--TEST--
Setting indirections must work for ArrayObject
--FILE--
<?php

// Based on test file for Bug #73686

echo "Using array\n";
$a = [];
foreach ([1, 2, 3] as $i => $var)
{
    $a[$i] = &$var;
}
$a[] = &$var;
var_dump($a);

echo "Using ArrayObject\n";
$ao = new ArrayObject;
foreach ([1, 2, 3] as $i => $var)
{
    $ao[$i] = &$var;
}
var_dump($ao);
$ao[] = &$var;
var_dump($ao);
?>
--EXPECT--
Using array
array(4) {
  [0]=>
  &int(3)
  [1]=>
  &int(3)
  [2]=>
  &int(3)
  [3]=>
  &int(3)
}
Using ArrayObject
object(ArrayObject)#1 (1) {
  ["storage":"ArrayObject":private]=>
  array(3) {
    [0]=>
    &int(3)
    [1]=>
    &int(3)
    [2]=>
    &int(3)
  }
}
object(ArrayObject)#1 (1) {
  ["storage":"ArrayObject":private]=>
  array(4) {
    [0]=>
    &int(3)
    [1]=>
    &int(3)
    [2]=>
    &int(3)
    [3]=>
    &int(3)
  }
}
