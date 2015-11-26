--TEST--
serialize()/unserialize() floats in array.
--INI--
precision=12
serialize_precision=100
--FILE--
<?php
error_reporting (E_ALL);
$a      = array(4);
$str    = serialize($a);
print('Serialized array: '.$str."\n");
$b      = unserialize($str);
print('Unserialized array: ');
var_dump($b);
print("\n");
$str    = serialize(array(4.5));
print('Serialized array: '.$str."\n");
$b      = unserialize($str);
print('Unserialized array: ')   ;
var_dump($b);
?>
--EXPECT--
Serialized array: a:1:{i:0;i:4;}
Unserialized array: array(1) {
  [0]=>
  int(4)
}

Serialized array: a:1:{i:0;d:4.5;}
Unserialized array: array(1) {
  [0]=>
  float(4.5)
}
