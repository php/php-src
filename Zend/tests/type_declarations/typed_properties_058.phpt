--TEST--
Constants in default values of properties
--FILE--
<?php
declare(strict_types=1);

define("FOO", 5);

class A {
	public int $foo = FOO;
}

class B {
	public string $foo = FOO;
}

try {
	$o = new A();
	var_dump($o->foo);
} catch (Throwable $e) {
	echo $e->getMessage() . "\n";
}

for ($i = 0; $i < 2; $i++) {
	try {
		$o = new B();
		var_dump($o->foo);
	} catch (Throwable $e) {
		echo $e->getMessage() . "\n";
	}
}
?>
--EXPECT--
int(5)
Typed property B::$foo must be string, int used
Typed property B::$foo must be string, int used

