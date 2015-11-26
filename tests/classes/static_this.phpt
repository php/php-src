--TEST--
ZE2 $this can be an argument to a static function
--FILE--
<?php

class TestClass
{
	function __construct()
	{
		self::Test1();
		$this->Test1();
	}

	static function Test1()
	{
		var_dump($this);
	}

	static function Test2($this)
	{
		var_dump($this);
	}
}

$obj = new TestClass;
TestClass::Test2(new stdClass);

?>
===DONE===
--EXPECTF--

Notice: Undefined variable: this in %sstatic_this.php on line %d
NULL

Notice: Undefined variable: this in %sstatic_this.php on line %d
NULL
object(stdClass)#%d (0) {
}
===DONE===
