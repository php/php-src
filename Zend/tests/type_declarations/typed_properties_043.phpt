--TEST--
Trying to assign to a static 'self' typed property on a trait must not fixate the type to the trait
--FILE--
<?php

trait Test {
	public static self $prop;
}

try {
	Test::$prop = new stdClass;
} catch (Error $e) {
	echo $e->getMessage(), "\n";
}

class Foo {
	use Test;
}

Foo::$prop = new Foo;

var_dump(Foo::$prop);

?>
--EXPECT--
Cannot write a value to a 'self' typed static property of a trait
object(Foo)#1 (0) {
}
