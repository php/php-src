--TEST--
wddx deserialization test
--FILE--
<?php
	$path = dirname(__FILE__);
	var_dump(wddx_deserialize(file_get_contents("{$path}/wddx.xml")));
?>
--EXPECT--
array(8) {
  ["aNull"]=>
  NULL
  ["aString"]=>
  string(8) "a string"
  ["aNumber"]=>
  float(-12.456)
  ["aBoolean"]=>
  bool(true)
  ["anArray"]=>
  array(2) {
    [0]=>
    int(10)
    [1]=>
    string(14) "second element"
  }
  ["aBinary"]=>
  string(11) "binary data"
  ["anObject"]=>
  array(2) {
    ["s"]=>
    string(8) "a string"
    ["n"]=>
    float(-12.456)
  }
  ["aRecordset"]=>
  array(2) {
    ["NAME"]=>
    array(2) {
      [0]=>
      string(8) "John Doe"
      [1]=>
      string(8) "Jane Doe"
    }
    ["AGE"]=>
    array(2) {
      [0]=>
      int(34)
      [1]=>
      int(31)
    }
  }
}
