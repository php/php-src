--TEST--
Bug #78868: Calling __autoload() with incorrect EG(fake_scope) value
--FILE--
<?php
class C {
	private $private = 1;

	function foo() {
		$this->private++; //fails with EG(fake_scope) != NULL && EG(fake_scope) != "C"
	}
}

class A {
	static $foo = B::foo; //not resolved on include()
}

function main_autoload($class_name) {
	$c = new C;
	$c->foo();
	//doesn't affect the error
	eval("class B {const foo = 1;}");
}

spl_autoload_register('main_autoload', false);

$classA = new ReflectionClass("A");
$props = $classA->getProperties();
$props[0]->setValue(2); //causes constant resolving, which runs autoload, all with EG(fake_scope) == "A"

echo "OK\n";
?>
--EXPECT--
OK
