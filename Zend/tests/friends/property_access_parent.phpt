--TEST--
Friends: allows access to properties via `parent::` in a property hook
--FILE--
<?php

class Foo {
	friend Bar;

	private $privateInstance;
}

class Bar extends Foo {

	public $privateInstance {
		set {
			parent::$privateInstance::set($value);
		}
	}

}

// Confirm that the presence of a friend does not negate normal visibility
// enforcement for non friends
$f = new Foo();
try {
	$f->privateInstance = 1;
} catch (Error $e) {
	echo $e . "\n\n";
}

echo "\n\n-----\n\n";

// But friend works
$b = new Bar();
$b->privateInstance = 2;
var_dump($b);

?>
--EXPECTF--
Error: Cannot access private property Foo::$privateInstance in %s:%d
Stack trace:
#0 {main}



-----

object(Bar)#%d (1) {
  ["privateInstance":"Foo":private]=>
  int(2)
}
