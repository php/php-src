--TEST--
First Class Callable returns correct name from is_callable()
--FILE--
<?php

class Foo {
	public function __invoke() {

	}

	public static function bar() { 

	}
}

function some_func() {

}

is_callable(some_func(...), callable_name: $name);
var_dump($name);

is_callable((new Foo())(...), callable_name: $name);
var_dump($name);

is_callable(Foo::bar(...), callable_name: $name);
var_dump($name);
?>
--EXPECT--
string(9) "some_func"
string(13) "Foo::__invoke"
string(8) "Foo::bar"
