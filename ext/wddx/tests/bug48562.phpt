--TEST--
Bug #48562 (Reference recursion causes segfault when used in wddx_serialize_vars())
--SKIPIF--
<?php
if (!extension_loaded('wddx')) {
    die('skip. wddx not available');
}
?>
--FILE--
<?php

$foo = 'bar';

$a['x'] = 'foo';
$a['x'] = &$a;

var_dump(wddx_serialize_vars($a));

// replace $a - the recursion detection seems to be causing $a to be not an array here, maybe its internally a pointer
// replacing $a with a new array() allows this test to still check for 2 things
//  1. recursion detection in &$a;
//  2. recursion detection in adding $a to itself and then serializing $a
// the one thing the test won't check is using $a as an array after doing &$a; which isn't really a wddx problem.
$a = array();
$a['x'] = 'foo';
$a['x'] = $a;

var_dump(wddx_serialize_vars($a));

?>
--EXPECTF--
Warning: wddx_serialize_vars(): recursion detected in %s on line %d
string(78) "<wddxPacket version='1.0'><header/><data><struct></struct></data></wddxPacket>"
string(120) "<wddxPacket version='1.0'><header/><data><struct><var name='foo'><string>bar</string></var></struct></data></wddxPacket>"
