--TEST--
Test that final properties can be used with key() and current(), but not with next()
--XFAIL--
Incorrect error message
--FILE--
<?php

class Foo
{
    final public array $property1 = ["foo", "bar", "baz"];
}

$foo = new Foo();

var_dump(current($foo->property1));
var_dump(key($foo->property1));

try {
    next($foo->property1);
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
string(3) "foo"
int(0)
Cannot modify final property Foo::$property1 after initialization
