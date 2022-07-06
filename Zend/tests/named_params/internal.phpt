--TEST--
Named params on internal functions
--FILE--
<?php

var_dump(array_slice(array: [1, 2, 3, 4, 5], offset: 2, length: 2));
var_dump(array_slice(length: 2, offset: 2, array: [1, 2, 3, 4, 5]));

var_dump(array_slice(array: ['a' => 0, 'b' => 1], offset: 1, preserve_keys: true));
var_dump(array_slice(['a' => 0, 'b' => 1], preserve_keys: true, offset: 1));
var_dump(str_pad("foo", 6, pad_type: STR_PAD_LEFT));

// Named params work with specialized functions.
var_dump(strlen(string: 'foo'));

?>
--EXPECT--
array(2) {
  [0]=>
  int(3)
  [1]=>
  int(4)
}
array(2) {
  [0]=>
  int(3)
  [1]=>
  int(4)
}
array(1) {
  ["b"]=>
  int(1)
}
array(1) {
  ["b"]=>
  int(1)
}
string(6) "   foo"
int(3)
