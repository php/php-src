--TEST--
GH-18581: Coerce numeric string keys from iterators when argument unpacking
--FILE--
<?php

function g() {
    yield '100' => 'first';
    yield '101' => 'second';
    yield '102' => 'third';
    yield 'named' => 'fourth';
}

function test($x = null, $y = null, ...$z) {
    var_dump($x, $y, $z);
    var_dump($z[0]);
    var_dump($z['named']);
}

test(...g());

?>
--EXPECT--
string(5) "first"
string(6) "second"
array(2) {
  [0]=>
  string(5) "third"
  ["named"]=>
  string(6) "fourth"
}
string(5) "third"
string(6) "fourth"
