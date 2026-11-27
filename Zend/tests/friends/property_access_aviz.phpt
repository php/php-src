--TEST--
Friends: allows access to protected properties with asymmetric visibility
--FILE--
<?php

class Foo {
	friend Bar;

	public protected(set) static mixed $protectedStatic;
	public private(set) static mixed $privateStatic = true;

	public protected(set) mixed $protectedInstance;
	public private(set) mixed $privateInstance;
}

class Bar {
	public static function testPropertyAccess() {
		Foo::$protectedStatic = 1;
		var_dump(Foo::$protectedStatic);
		echo "\n";
		try {
			Foo::$privateStatic = 2;
		} catch (Error $e) {
			echo $e . "\n\n";
		}
		var_dump(Foo::$privateStatic);
		echo "\n";
		$f = new Foo();
		$f->protectedInstance = 3;
		try {
			$f->privateInstance = 4;
		} catch (Error $e) {
			echo $e . "\n\n";
		}
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

Error: Cannot modify private(set) property Foo::$privateStatic from scope Bar in %s:%d
Stack trace:
#0 %s(%d): Bar::testPropertyAccess()
#1 {main}

bool(true)

Error: Cannot modify private(set) property Foo::$privateInstance from scope Bar in %s:%d
Stack trace:
#0 %s(%d): Bar::testPropertyAccess()
#1 {main}

object(Foo)#%d (1) {
  ["protectedInstance"]=>
  int(3)
  ["privateInstance"]=>
  uninitialized(mixed)
}
