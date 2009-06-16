--TEST--
Bug #48562 (Reference recursion causes segfault when used in wddx_serialize_vars())
--FILE--
<?php

$foo = 'bar';

$a['x'] = 'foo';
$a['x'] = &$a;

var_dump(wddx_serialize_vars($a));

$a['x'] = 'foo';
$a['x'] = $a;

var_dump(wddx_serialize_vars($a));

?>
--EXPECTF--
Warning: wddx_serialize_vars(): recursion detected in %s on line %d
string(78) "<wddxPacket version='1.0'><header/><data><struct></struct></data></wddxPacket>"
string(120) "<wddxPacket version='1.0'><header/><data><struct><var name='foo'><string>bar</string></var></struct></data></wddxPacket>"
