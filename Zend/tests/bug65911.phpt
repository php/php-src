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
Fatal error: Uncaught Error: Access to undeclared static property: A::$this in %s:%d
Stack trace:
#0 %s(%d): B->go()
#1 {main}
  thrown in %s on line %d
