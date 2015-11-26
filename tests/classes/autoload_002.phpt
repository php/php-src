--TEST--
ZE2 Autoload and get_class_methods
--SKIPIF--
<?php 
	if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); 
	if (class_exists('autoload_root', false)) die('skip Autoload test classes exist already');
?>
--FILE--
<?php

function __autoload($class_name)
{
	require_once(dirname(__FILE__) . '/' . $class_name . '.p5c');
	echo __FUNCTION__ . '(' . $class_name . ")\n";
}

var_dump(get_class_methods('autoload_root'));

?>
===DONE===
--EXPECT--
__autoload(autoload_root)
array(1) {
  [0]=>
  string(12) "testFunction"
}
===DONE===
