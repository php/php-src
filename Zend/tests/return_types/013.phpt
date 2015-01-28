--TEST--
Closure inside method returned null, expected array

--FILE--
<?php
class foo {
    public function bar() : callable {
        $test = "one";
        return function() use($test) : array {
            return null;
        };
    }
}

$baz = new foo();
var_dump($func=$baz->bar(), $func());

--EXPECTF--
Catchable fatal error: Return value of foo::{closure}() must be of the type array, null returned in %s on line %d
