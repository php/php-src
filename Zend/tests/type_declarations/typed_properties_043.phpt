--TEST--
Constants in default values of properties with weak types
--FILE--
<?php

define("FOO", 5e20);

class A {
	public string $foo = FOO;
	public float $float = FOO;
}

class B {
	public int $foo = FOO;
}

$o = new A();
var_dump($o->foo, $o->float);

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
string(7) "5.0E+20"
float(5.0E+20)
Typed property B::$foo must be int, float used
Typed property B::$foo must be int, float used
