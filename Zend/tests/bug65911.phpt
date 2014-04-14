--TEST--
Bug #65911 (scope resolution operator - strange behavior with $this)
--FILE--
<?php
class A {}

class B
{
	public function go()
	{
		$this->foo = 'bar';
		echo A::$this->foo; // should not output 'bar'
	}
}

$obj = new B();
$obj->go();
?>
--EXPECTF--
Fatal error: Access to undeclared static property: A::$this in %s on line %d
