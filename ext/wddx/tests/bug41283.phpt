--TEST--
Bug #41283 (Bug with serializing array key that are doubles or floats)
--SKIPIF--
<?php if (!extension_loaded("wddx")) print "skip"; ?>
--FILE--
<?php
$data = array(
  'somearray' => array('1.1' => 'One 1','1.2' => 'One 2', '1.0' => 'Three')
);

var_dump(wddx_deserialize(wddx_serialize_vars('data')));
?>
--EXPECT--
array(1) {
  ["data"]=>
  array(1) {
    ["somearray"]=>
    array(3) {
      ["1.1"]=>
      string(5) "One 1"
      ["1.2"]=>
      string(5) "One 2"
      ["1.0"]=>
      string(5) "Three"
    }
  }
}
