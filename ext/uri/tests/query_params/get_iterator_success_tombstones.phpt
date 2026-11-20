--TEST--
Test Uri\QueryParams::getIterator() - consecutive keys after deleting entries
--FILE--
<?php

$params = (new Uri\QueryParams())
    ->append("x", "0")->append("y", "first")
    ->append("x", "1")->append("y", "second")
    ->append("x", "2")->append("z", "last")->append("x", "3")
    ->delete("x");

var_dump(iterator_to_array($params) === $params->toList());

$iterator = $params->getIterator();
var_dump($iterator->key(), $iterator->current());
$iterator->next();
$iterator->next();
var_dump($iterator->key(), $iterator->current());
$iterator->next();
var_dump($iterator->valid());
$iterator->rewind();
var_dump($iterator->key(), $iterator->current());

?>
--EXPECT--
bool(true)
int(0)
array(2) {
  [0]=>
  string(1) "y"
  [1]=>
  string(5) "first"
}
int(2)
array(2) {
  [0]=>
  string(1) "z"
  [1]=>
  string(4) "last"
}
bool(false)
int(0)
array(2) {
  [0]=>
  string(1) "y"
  [1]=>
  string(5) "first"
}
