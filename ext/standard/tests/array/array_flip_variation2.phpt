--TEST--
Test array_flip() function : usage variations - 'input' array with different keys
--FILE--
<?php
/*
* Trying different keys in $input array argument
*/

echo "*** Testing array_flip() : different keys for 'input' array argument ***\n";

$input = array(
  // default key
  'one',  //expected: default key 0, value will be replaced by 'bool_key2'

  // numeric keys
  1 => 'int_key', // expected: value will be replaced by 'bool_key1'
  -2 => 'negative_key',
  012 => 'octal_key',
  0x34 => 'hex_key',

  // string keys
  'key' => 'string_key1',
  "two" => 'string_key2',
  '' => 'string_key3',
  "" => 'string_key4',
  " " => 'string_key5',

  // bool keys
  true => 'bool_key1',
  false => 'bool_key2',

  // binary key
  "a".chr(0)."b" => 'binary_key1',
  b"binary" => 'binary_key2',
);

var_dump( array_flip($input) );

echo "Done"
?>
--EXPECTF--
*** Testing array_flip() : different keys for 'input' array argument ***
array(11) {
  ["bool_key2"]=>
  int(0)
  ["bool_key1"]=>
  int(1)
  ["negative_key"]=>
  int(-2)
  ["octal_key"]=>
  int(10)
  ["hex_key"]=>
  int(52)
  ["string_key1"]=>
  string(3) "key"
  ["string_key2"]=>
  string(3) "two"
  ["string_key4"]=>
  string(0) ""
  ["string_key5"]=>
  string(1) " "
  ["binary_key1"]=>
  string(3) "a%0b"
  ["binary_key2"]=>
  string(6) "binary"
}
Done
