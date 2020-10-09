--TEST--
Bug #30234 (__autoload() not invoked for interfaces)
--SKIPIF--
<?php
    if (class_exists('autoload_root', false)) die('skip Autoload test classes exist already');
?>
--FILE--
<?php

spl_autoload_register(function ($class_name) {
    require_once(__DIR__ . '/' . strtolower($class_name) . '.inc');
    echo __FUNCTION__ . '(' . $class_name . ")\n";
});

var_dump(interface_exists('autoload_interface', false));
var_dump(class_exists('autoload_implements', false));

$o = unserialize('O:19:"Autoload_Implements":0:{}');

var_dump($o);
var_dump($o instanceof autoload_interface);
unset($o);

var_dump(interface_exists('autoload_interface', false));
var_dump(class_exists('autoload_implements', false));

?>
--EXPECTF--
bool(false)
bool(false)
{closure}(autoload_interface)
{closure}(Autoload_Implements)
object(autoload_implements)#%d (0) {
}
bool(true)
bool(true)
bool(true)
