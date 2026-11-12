--TEST--
ArrayObject and ArrayIterator share arrays they hand out copy-on-write
--FILE--
<?php
// Arrays handed out by getArrayCopy(), exchangeArray(), clone, (array) and
// __serialize() are independent of the object they come from

$ao = new ArrayObject(['a' => 1, 'b' => 2]);
$copy = $ao->getArrayCopy();
$ao['a'] = 10;
$copy['b'] = 20;
var_dump($ao->getArrayCopy(), $copy);

$ao = new ArrayObject(['a' => 1]);
$clone = clone $ao;
$clone['a'] = 2;
$ao['b'] = 3;
var_dump($ao->getArrayCopy(), $clone->getArrayCopy());

$ao = new ArrayObject(['a' => 1]);
$cast = (array) $ao;
$ao['a'] = 2;
$cast['b'] = 3;
var_dump($ao->getArrayCopy(), $cast);

$source = ['a' => 1];
$ao = new ArrayObject($source);
$old = $ao->exchangeArray($source);
$old['b'] = 2;
$ao['c'] = 3;
var_dump($source, $old, $ao->getArrayCopy());

$it = new ArrayIterator(['a' => 1]);
$copy = $it->getArrayCopy();
$it2 = new ArrayIterator($copy);
$it2['a'] = 2;
$copy['a'] = 3;
var_dump($it['a'], $it2['a'], $copy['a']);

$ao = new ArrayObject(['a' => 1]);
$it = $ao->getIterator();
$copy = $it->getArrayCopy();
$it['a'] = 2;
$copy['b'] = 3;
var_dump($ao->getArrayCopy(), $copy);

// Several objects built from the same array
$source = [1, 2, 3];
$objects = [new ArrayIterator($source), new ArrayObject($source), new ArrayObject($source)];
$objects[0][0] = 'it';
$objects[1][] = 'ao';
unset($objects[2][2]);
foreach ($objects as $object) {
    echo json_encode($object->getArrayCopy()), "\n";
}
echo json_encode($source), "\n";
?>
--EXPECT--
array(2) {
  ["a"]=>
  int(10)
  ["b"]=>
  int(2)
}
array(2) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(20)
}
array(2) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(3)
}
array(1) {
  ["a"]=>
  int(2)
}
array(1) {
  ["a"]=>
  int(2)
}
array(2) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(3)
}
array(1) {
  ["a"]=>
  int(1)
}
array(2) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
}
array(2) {
  ["a"]=>
  int(1)
  ["c"]=>
  int(3)
}
int(1)
int(2)
int(3)
array(1) {
  ["a"]=>
  int(2)
}
array(2) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(3)
}
["it",2,3]
[1,2,3,"ao"]
[1,2]
[1,2,3]
