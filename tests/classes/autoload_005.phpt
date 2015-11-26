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
	var_dump(class_exists($class_name, false));
	require_once(dirname(__FILE__) . '/' . $class_name . '.p5c');
	echo __FUNCTION__ . '(' . $class_name . ")\n";
}

var_dump(class_exists('autoload_derived', false));
var_dump(class_exists('autoload_derived', false));

class Test
{
    function __destruct() {
        echo __METHOD__ . "\n";
        $o = new autoload_derived;
        var_dump($o);
    }
}

$o = new Test;
unset($o);

?>
===DONE===
--EXPECTF--
bool(false)
bool(false)
Test::__destruct
bool(false)
bool(false)
__autoload(autoload_root)
__autoload(autoload_derived)
object(autoload_derived)#%d (0) {
}
===DONE===
