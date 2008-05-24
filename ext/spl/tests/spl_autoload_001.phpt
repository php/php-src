--TEST--
SPL: spl_autoload() and friends
--INI--
include_path=.
--FILE--
<?php

echo "===EMPTY===\n";

var_dump(spl_autoload_extensions());

try
{
	spl_autoload("TestClass");
}
catch(Exception $e)
{
	echo 'Exception: ' . $e->getMessage() . "\n";
}

$test_exts = array(NULL, "1", ".inc,,.php.inc", "");

foreach($test_exts as $exts)
{
	echo "===($exts)===\n";
	try
	{
		spl_autoload("TestClass", $exts);
	}
	catch(Exception $e)
	{
		echo 'Exception: ' . $e->getMessage() . "\n";
	}
}

try
{
	spl_autoload_extensions(".inc,.php.inc");
	spl_autoload("TestClass");
}
catch(Exception $e)
{
	echo 'Exception: ' . $e->getMessage() . "\n";
}

function TestFunc1($classname)
{
	echo __METHOD__ . "($classname)\n";
}

function TestFunc2($classname)
{
	echo __METHOD__ . "($classname)\n";
}

echo "===SPL_AUTOLOAD()===\n";

spl_autoload_register();

try
{
	var_dump(spl_autoload_extensions(".inc"));
	var_dump(class_exists("TestClass", true));
}
catch(Exception $e)
{
	echo 'Exception: ' . $e->getMessage() . "\n";
}

echo "===REGISTER===\n";

spl_autoload_unregister("spl_autoload");
spl_autoload_register("TestFunc1");
spl_autoload_register("TestFunc2");
spl_autoload_register("TestFunc2"); /* 2nd call ignored */
spl_autoload_extensions(".inc,.class.inc"); /* we do not have spl_autoload_registered yet */

try
{
	var_dump(class_exists("TestClass", true));
}
catch(Exception $e)
{
	echo 'Exception: ' . $e->getMessage() . "\n";
}

echo "===LOAD===\n";

spl_autoload_register("spl_autoload");
var_dump(class_exists("TestClass", true));

echo "===NOFUNCTION===\n";

try
{
	spl_autoload_register("unavailable_autoload_function");
}
catch(Exception $e)
{
	echo 'Exception: ' . $e->getMessage() . "\n";
}

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
===EMPTY===
string(9) ".inc,.php"
%stestclass.inc
Exception: Class TestClass could not be loaded
===()===
Exception: Class TestClass could not be loaded
===(1)===
Exception: Class TestClass could not be loaded
===(.inc,,.php.inc)===
%stestclass
%stestclass.php.inc
Exception: Class TestClass could not be loaded
===()===
Exception: Class TestClass could not be loaded
Exception: Class TestClass could not be loaded
===SPL_AUTOLOAD()===
string(4) ".inc"
Exception: Class TestClass could not be loaded
===REGISTER===
TestFunc1(TestClass)
TestFunc2(TestClass)
bool(false)
===LOAD===
TestFunc1(TestClass)
TestFunc2(TestClass)
%stestclass.class.inc
bool(true)
===NOFUNCTION===
Exception: Function 'unavailable_autoload_function' not found (function 'unavailable_autoload_function' not found or invalid function name)
===DONE===
