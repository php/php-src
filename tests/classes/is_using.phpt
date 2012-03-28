--TEST--
ZE2 Verifying if a class use a given trait with is_using
--FILE--
<?php

spl_autoload_register(function() { var_dump(func_get_args()); });

trait Foo {

}

class Bar {
	use Foo;
}

$b = new Bar();
// true
var_dump(is_using('Bar', 'Foo'));
// true
var_dump(is_using($b, 'Foo'));
// autoload then false
var_dump(is_using('Bar2', 'Foo'));
// false
var_dump(is_using($b, 'Foo2'));

?>
--EXPECT--
bool(true)
bool(true)
array(1) {
  [0]=>
  string(4) "Bar2"
}
bool(false)
bool(false)
