--TEST--
Bug #53515 (property_exists incorrect on ArrayObject null and 0 values)
--FILE--
<?php

$a = array('a' => 1, 'b'=> true, 'c' => 0, 'd' => null, 'e' => false, 'f' => array());
$o = new ArrayObject($a, ArrayObject::ARRAY_AS_PROPS);

$a['z'] = '';
$a[''] = '';

foreach ($a as $key => $value) {
 echo $key . ': ' . (is_null($value) ? 'null' : @"$value") .
    ' array_key_exists: ' . (array_key_exists($key, $a) ? 'true' : 'false') . 
    ' property_exists: ' . (property_exists($o, $key) ? 'true' : 'false'),"\n";
}

?>
--EXPECT--
a: 1 array_key_exists: true property_exists: true
b: 1 array_key_exists: true property_exists: true
c: 0 array_key_exists: true property_exists: true
d: null array_key_exists: true property_exists: true
e:  array_key_exists: true property_exists: true
f: Array array_key_exists: true property_exists: true
z:  array_key_exists: true property_exists: false
:  array_key_exists: true property_exists: false
