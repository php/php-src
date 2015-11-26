--TEST--
Generators work properly in MultipleIterator
--FILE--
<?php

function gen1() {
    yield 'a';
    yield 'aa';
}

function gen2() {
    yield 'b';
    yield 'bb';
}

$it = new MultipleIterator;
$it->attachIterator(gen1());
$it->attachIterator(gen2());

foreach ($it as $values) {
    var_dump($values);
}

?>
--EXPECT--
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
}
array(2) {
  [0]=>
  string(2) "aa"
  [1]=>
  string(2) "bb"
}
