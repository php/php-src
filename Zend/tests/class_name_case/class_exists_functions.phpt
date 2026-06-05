--TEST--
Class name with incorrect case returns false in class_exists, interface_exists, trait_exists
--FILE--
<?php
class Foo {}
interface IFoo {}
trait TFoo {}

// correct case
var_dump(class_exists('Foo'));
var_dump(interface_exists('IFoo'));
var_dump(trait_exists('TFoo'));

// wrong case - returns false
var_dump(class_exists('FOO'));
var_dump(interface_exists('IFOO'));
var_dump(trait_exists('TFOO'));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
