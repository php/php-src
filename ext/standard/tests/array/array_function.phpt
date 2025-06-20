--TEST--
Test array() function
--FILE--
<?php
var_dump(\array(1, 2, 3));
var_dump(\array(foo: "bar", baz: "quux"));
var_dump(array(foo: "bar", baz: "quux"));

try {
	assert(false && \array(foo: "bar", baz: "quux"));
} catch (AssertionError $e) {
	echo $e->getMessage(), PHP_EOL;
}

var_dump(array_map(array(...), [1, 2, 3]));

?>
--EXPECT--
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(2) {
  ["foo"]=>
  string(3) "bar"
  ["baz"]=>
  string(4) "quux"
}
array(2) {
  ["bar"]=>
  string(3) "foo"
  ["quux"]=>
  string(3) "baz"
}
assert(false && \array(foo: 'bar', baz: 'quux'))
array(3) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
  [1]=>
  array(1) {
    [0]=>
    int(2)
  }
  [2]=>
  array(1) {
    [0]=>
    int(3)
  }
}
