--TEST--
Class types differing only in case are distinct in union and intersection types
--FILE--
<?php
class Foo {}
class foo {}

function test(Foo|foo $x): string {
    return get_class($x);
}

var_dump(test(new Foo()));
var_dump(test(new foo()));

try {
    test(new stdClass());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
string(3) "Foo"
string(3) "foo"
test(): Argument #1 ($x) must be of type Foo|foo, stdClass given, called in %s on line %d