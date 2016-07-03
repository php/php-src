--TEST--
serialize()/unserialize() objects
--SKIPIF--
<?php if (!interface_exists('Serializable')) die('skip Interface Serialzable not defined'); ?>
--FILE--
<?php

// This test verifies that old and new style (un)serializing do not interfere.

function do_autoload($class_name)
{
	if ($class_name != 'autoload_not_available')
	{
		require_once(dirname(__FILE__) . '/' . strtolower($class_name) . '.p5c');
	}
	echo __FUNCTION__ . "($class_name)\n";
}

function unserializer($class_name)
{
	echo __METHOD__ . "($class_name)\n";
	switch($class_name)
	{
	case 'TestNAOld':
		eval("class TestNAOld extends TestOld {}");
		break;
	case 'TestNANew':
		eval("class TestNANew extends TestNew {}");
		break;
	case 'TestNANew2':
		eval("class TestNANew2 extends TestNew {}");
		break;
	default:
		echo "Try __autoload()\n";
		if (!function_exists('__autoload'))
		{
			eval('function __autoload($class_name) { do_autoload($class_name); }');
		}
		__autoload($class_name);
		break;
	}
}

ini_set('unserialize_callback_func', 'unserializer');

class TestOld
{
	function serialize()
	{
		echo __METHOD__ . "()\n";
	}
	
	function unserialize($serialized)
	{
		echo __METHOD__ . "()\n";
	}
	
	function __wakeup()
	{
		echo __METHOD__ . "()\n";
	}
	
	function __sleep()
	{
		echo __METHOD__ . "()\n";
		return array();
	}
}

class TestNew implements Serializable
{
	protected static $check = 0;

	function serialize()
	{
		echo __METHOD__ . "()\n";
		switch(++self::$check)
		{
		case 1:
			return NULL;
		case 2:
			return "2";
		}
	}
	
	function unserialize($serialized)
	{
		echo __METHOD__ . "()\n";
	}
	
	function __wakeup()
	{
		echo __METHOD__ . "()\n";
	}
	
	function __sleep()
	{
		echo __METHOD__ . "()\n";
	}
}

echo "===O1===\n";
var_dump($ser = serialize(new TestOld));
var_dump(unserialize($ser));

echo "===N1===\n";
var_dump($ser = serialize(new TestNew));
var_dump(unserialize($ser));

echo "===N2===\n";
var_dump($ser = serialize(new TestNew));
var_dump(unserialize($ser));

echo "===NAOld===\n";
var_dump(unserialize('O:9:"TestNAOld":0:{}'));

echo "===NANew===\n";
var_dump(unserialize('O:9:"TestNANew":0:{}'));

echo "===NANew2===\n";
var_dump(unserialize('C:10:"TestNANew2":0:{}'));

echo "===AutoOld===\n";
var_dump(unserialize('O:19:"autoload_implements":0:{}'));

// Now we have __autoload(), that will be called before the old style header.
// If the old style handler also fails to register the class then the object
// becomes an incomplete class instance.

echo "===AutoNA===\n";
var_dump(unserialize('O:22:"autoload_not_available":0:{}'));
?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
===O1===
TestOld::__sleep()
string(18) "O:7:"TestOld":0:{}"
TestOld::__wakeup()
object(TestOld)#%d (0) {
}
===N1===
TestNew::serialize()
string(2) "N;"
NULL
===N2===
TestNew::serialize()
string(19) "C:7:"TestNew":1:{2}"
TestNew::unserialize()
object(TestNew)#%d (0) {
}
===NAOld===
unserializer(TestNAOld)
TestOld::__wakeup()
object(TestNAOld)#%d (0) {
}
===NANew===
unserializer(TestNANew)

Warning: Erroneous data format for unserializing 'TestNANew' in %s005.php on line %d

Notice: unserialize(): Error at offset 19 of 20 bytes in %s005.php on line %d
bool(false)
===NANew2===
unserializer(TestNANew2)
TestNew::unserialize()
object(TestNANew2)#%d (0) {
}
===AutoOld===
unserializer(autoload_implements)
Try __autoload()
do_autoload(autoload_interface)
do_autoload(autoload_implements)
object(autoload_implements)#%d (0) {
}
===AutoNA===
do_autoload(autoload_not_available)
unserializer(autoload_not_available)
Try __autoload()
do_autoload(autoload_not_available)
do_autoload(autoload_not_available)

Warning: unserialize(): Function unserializer() hasn't defined the class it was called for in %s005.php on line %d
object(__PHP_Incomplete_Class)#%d (1) {
  ["__PHP_Incomplete_Class_Name"]=>
  string(22) "autoload_not_available"
}
===DONE===
