--TEST--
Reflection Bug #30209 (ReflectionClass::getMethod() lowercases attribute)
--FILE--
<?php

class Foo
{
	private $name = 'testBAR';

	public function testBAR()
	{
		try
		{
			$class  = new ReflectionClass($this);
			var_dump($this->name);
			$method = $class->getMethod($this->name);
			var_dump($this->name);
		}

		catch (Exception $e) {}
	}
}

$foo = new Foo;
$foo->testBAR();
?>
===DONE===
--EXPECT--
string(7) "testBAR"
string(7) "testBAR"
===DONE===
