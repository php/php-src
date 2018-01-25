--TEST--
SPL: spl_autoload() with exceptions
--INI--
include_path=.
--FILE--
<?php

function MyAutoLoad($className)
{
	echo __METHOD__ . "($className)\n";
	throw new Exception('Bla');
}

class MyAutoLoader
{
	static function autoLoad($className)
	{
		echo __METHOD__ . "($className)\n";
		throw new Exception('Bla');
	}

	function dynaLoad($className)
	{
		echo __METHOD__ . "($className)\n";
		throw new Exception('Bla');
	}
}

$obj = new MyAutoLoader;

$funcs = array(
	'MyAutoLoad',
	'MyAutoLoader::autoLoad',
	'MyAutoLoader::dynaLoad',
	array('MyAutoLoader', 'autoLoad'),
	array('MyAutoLoader', 'dynaLoad'),
	array($obj, 'autoLoad'),
	array($obj, 'dynaLoad'),
);

foreach($funcs as $idx => $func)
{
	echo "====$idx====\n";

	try
	{
		var_dump($func);
		spl_autoload_register($func);
		if (count(spl_autoload_functions()))
		{
			echo "registered\n";

			var_dump(class_exists("NoExistingTestClass", true));
		}
	}
	catch (Exception $e)
	{
		echo get_class($e) . ": " . $e->getMessage() . "\n";
	}

	spl_autoload_unregister($func);
	var_dump(count(spl_autoload_functions()));
}

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
====0====
string(10) "MyAutoLoad"
registered
MyAutoLoad(NoExistingTestClass)
Exception: Bla
int(0)
====1====
string(22) "MyAutoLoader::autoLoad"
registered
MyAutoLoader::autoLoad(NoExistingTestClass)
Exception: Bla
int(0)
====2====
string(22) "MyAutoLoader::dynaLoad"
LogicException: Function 'MyAutoLoader::dynaLoad' not callable (non-static method MyAutoLoader::dynaLoad() should not be called statically)
int(0)
====3====
array(2) {
  [0]=>
  string(12) "MyAutoLoader"
  [1]=>
  string(8) "autoLoad"
}
registered
MyAutoLoader::autoLoad(NoExistingTestClass)
Exception: Bla
int(0)
====4====
array(2) {
  [0]=>
  string(12) "MyAutoLoader"
  [1]=>
  string(8) "dynaLoad"
}
LogicException: Passed array specifies a non static method but no object (non-static method MyAutoLoader::dynaLoad() should not be called statically)
int(0)
====5====
array(2) {
  [0]=>
  object(MyAutoLoader)#%d (0) {
  }
  [1]=>
  string(8) "autoLoad"
}
registered
MyAutoLoader::autoLoad(NoExistingTestClass)
Exception: Bla
int(0)
====6====
array(2) {
  [0]=>
  object(MyAutoLoader)#%d (0) {
  }
  [1]=>
  string(8) "dynaLoad"
}
registered
MyAutoLoader::dynaLoad(NoExistingTestClass)
Exception: Bla
int(0)
===DONE===
