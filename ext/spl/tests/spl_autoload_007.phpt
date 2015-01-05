--TEST--
SPL: spl_autoload() with inaccessible methods
--INI--
include_path=.
--FILE--
<?php

class MyAutoLoader {

        static protected function noAccess($className) {
        	echo __METHOD__ . "($className)\n";
        }

        static function autoLoad($className) {
        	echo __METHOD__ . "($className)\n";
        }

        function dynaLoad($className) {
        	echo __METHOD__ . "($className)\n";
        }
}

$obj = new MyAutoLoader;

$funcs = array(
	'MyAutoLoader::notExist',
	'MyAutoLoader::noAccess',
	'MyAutoLoader::autoLoad',
	'MyAutoLoader::dynaLoad',
	array('MyAutoLoader', 'notExist'),
	array('MyAutoLoader', 'noAccess'),
	array('MyAutoLoader', 'autoLoad'),
	array('MyAutoLoader', 'dynaLoad'),
	array($obj, 'notExist'),
	array($obj, 'noAccess'),
	array($obj, 'autoLoad'),
	array($obj, 'dynaLoad'),
);

foreach($funcs as $idx => $func)
{
	if ($idx) echo "\n";
	try
	{
		var_dump($func);
		spl_autoload_register($func);
		echo "ok\n";
	}
	catch (Exception $e)
	{
		echo $e->getMessage() . "\n";
	}
}

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
string(22) "MyAutoLoader::notExist"
Function 'MyAutoLoader::notExist' not found (class 'MyAutoLoader' does not have a method 'notExist')

string(22) "MyAutoLoader::noAccess"
Function 'MyAutoLoader::noAccess' not callable (cannot access protected method MyAutoLoader::noAccess())

string(22) "MyAutoLoader::autoLoad"
ok

string(22) "MyAutoLoader::dynaLoad"
Function 'MyAutoLoader::dynaLoad' not callable (non-static method MyAutoLoader::dynaLoad() should not be called statically)

array(2) {
  [0]=>
  string(12) "MyAutoLoader"
  [1]=>
  string(8) "notExist"
}
Passed array does not specify an existing static method (class 'MyAutoLoader' does not have a method 'notExist')

array(2) {
  [0]=>
  string(12) "MyAutoLoader"
  [1]=>
  string(8) "noAccess"
}
Passed array does not specify a callable static method (cannot access protected method MyAutoLoader::noAccess())

array(2) {
  [0]=>
  string(12) "MyAutoLoader"
  [1]=>
  string(8) "autoLoad"
}
ok

array(2) {
  [0]=>
  string(12) "MyAutoLoader"
  [1]=>
  string(8) "dynaLoad"
}
Passed array specifies a non static method but no object (non-static method MyAutoLoader::dynaLoad() should not be called statically)

array(2) {
  [0]=>
  object(MyAutoLoader)#%d (0) {
  }
  [1]=>
  string(8) "notExist"
}
Passed array does not specify an existing method (class 'MyAutoLoader' does not have a method 'notExist')

array(2) {
  [0]=>
  object(MyAutoLoader)#%d (0) {
  }
  [1]=>
  string(8) "noAccess"
}
Passed array does not specify a callable method (cannot access protected method MyAutoLoader::noAccess())

array(2) {
  [0]=>
  object(MyAutoLoader)#%d (0) {
  }
  [1]=>
  string(8) "autoLoad"
}
ok

array(2) {
  [0]=>
  object(MyAutoLoader)#%d (0) {
  }
  [1]=>
  string(8) "dynaLoad"
}
ok
===DONE===
