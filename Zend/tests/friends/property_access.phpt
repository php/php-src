--TEST--
Friends: allows access to properties
--FILE--
<?php

class Foo {
	friend Bar;

	protected static $protectedStatic;
	private static $privateStatic;

	protected $protectedInstance;
	private $privateInstance;
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
	Foo::$privateInstance = 2;
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
Error: Cannot access protected property Foo::$protectedStatic in %s:%d
Stack trace:
#0 {main}

Error: Cannot access private property Foo::$privateInstance in %s:%d
Stack trace:
#0 {main}

Error: Cannot access protected property Foo::$protectedInstance in %s:%d
Stack trace:
#0 {main}

Error: Cannot access private property Foo::$privateInstance in %s:%d
Stack trace:
#0 {main}



-----


Fatal error: Uncaught Error: Cannot access protected property Foo::$protectedStatic in %s:%d
Stack trace:
#0 %s(%d): Bar::testPropertyAccess()
#1 {main}
  thrown in %s on line %d
