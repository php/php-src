--TEST--
Bug #31102 (Exception not handled when thrown inside __autoload())
--FILE--
<?php

$test = 0;

function __autoload($class)
{
	global $test;

	echo __METHOD__ . "($class,$test)\n";
	switch($test)
	{
	case 1:
		eval("class $class { function __construct(){throw new Exception('$class::__construct');}}");
		return;
	case 2:
		eval("class $class { function __construct(){throw new Exception('$class::__construct');}}");
		throw new Exception(__METHOD__);
		return;
	case 3:
		return;
	}
}

while($test++ < 5)
{
	try
	{
		eval("\$bug = new Test$test();");
	}
	catch (Exception $e)
	{
		echo "Caught: " . $e->getMessage() . "\n";
	}
}
?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
__autoload(Test1,1)
Caught: Test1::__construct
__autoload(Test2,2)
Caught: __autoload
__autoload(Test3,3)

Fatal error: Class 'Test3' not found in %sbug31102.php(%d) : eval()'d code on line 1
