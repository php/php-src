--TEST--
Friends: allows access to properties with asymmetric visibility
--FILE--
<?php

class Foo {
	friend Bar;

	public protected(set) static mixed $protectedStatic;
	public private(set) static mixed $privateStatic;

	public protected(set) mixed $protectedInstance;
	public private(set) mixed $privateInstance;
}

class Bar {
	public static function testPropertyAccess() {
		Foo::$protectedStatic = 1;
		var_dump(Foo::$protectedStatic);
		Foo::$privateStatic = 2;
		var_dump(Foo::$privateStatic);
		$f = new Foo();
		$f->protectedInstance = 3;
		$f->privateInstance = 4;
		var_dump($f);
	}
}

// Confirm that the presence of a friend does not negate normal visibility
// enforcement for non friends
try {
	Foo::$protectedStatic = 1;
} catch (Error $e) {
	echo $e . "\n\n";
}
try {
	Foo::$privateStatic = 2;
} catch (Error $e) {
	echo $e . "\n\n";
}
$f = new Foo();
try {
	$f->protectedInstance = 3;
} catch (Error $e) {
	echo $e . "\n\n";
}
try {
	$f->privateInstance = 4;
} catch (Error $e) {
	echo $e . "\n\n";
}

echo "\n\n-----\n\n";

// But friend works
Bar::testPropertyAccess();

?>
--EXPECTF--
Error: Cannot modify protected(set) property Foo::$protectedStatic from global scope in %s:%d
Stack trace:
#0 {main}

Error: Cannot modify private(set) property Foo::$privateStatic from global scope in %s:%d
Stack trace:
#0 {main}

Error: Cannot modify protected(set) property Foo::$protectedInstance from global scope in %s:%d
Stack trace:
#0 {main}

Error: Cannot modify private(set) property Foo::$privateInstance from global scope in %s:%d
Stack trace:
#0 {main}



-----

int(1)
int(2)
object(Foo)#%d (2) {
  ["protectedInstance"]=>
  int(3)
  ["privateInstance"]=>
  int(4)
}
