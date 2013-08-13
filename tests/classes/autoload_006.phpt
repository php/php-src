--TEST--
ZE2 Autoload from destructor
--SKIPIF--
<?php 
	if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); 
	if (class_exists('autoload_root', false)) die('skip Autoload test classes exist already');
?>
--FILE--
<?php

function __autoload($class_name)
{
	require_once(dirname(__FILE__) . '/' . strtolower($class_name) . '.p5c');
	echo __FUNCTION__ . '(' . $class_name . ")\n";
}

var_dump(interface_exists('autoload_interface', false));
var_dump(class_exists('autoload_implements', false));

$o = new Autoload_Implements;
var_dump($o);
var_dump($o instanceof autoload_interface);
unset($o);

var_dump(interface_exists('autoload_interface', false));
var_dump(class_exists('autoload_implements', false));

?>
===DONE===
--EXPECTF--
bool(false)
bool(false)
__autoload(autoload_interface)
__autoload(Autoload_Implements)
object(autoload_implements)#%d (0) {
}
bool(true)
bool(true)
bool(true)
===DONE===
