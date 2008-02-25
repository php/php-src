--TEST--
wddx deserialization test (64-bit)
--SKIPIF--
<?php if (!extension_loaded("wddx")) print "skip"; ?>
<?php echo PHP_INT_SIZE != 8 ? "skip 64-bit only" : "OK" ?>
--INI--
precision=14
--FILE--
<?php
	$path = dirname(__FILE__);
	var_dump(wddx_deserialize(file_get_contents("{$path}/wddx.xml")));
?>
--EXPECT--
array(11) {
  ["aNull"]=>
  NULL
  ["aString"]=>
  string(8) "a string"
  ["aNumber"]=>
  float(-12.456)
  ["aDateTime"]=>
  int(897625932)
  ["aDateTime2"]=>
  int(329632332)
  ["aDateTime3"]=>
  int(2223088332)
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
