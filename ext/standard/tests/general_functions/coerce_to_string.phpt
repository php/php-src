--TEST--
coerce_to_string()
--FILE--
<?php

class Foo {
    public function __toString() {
        return 'Bar';
    }
}

class Bar {}

$foo = new Foo;
$ref = &$ref;

var_dump(coerce_to_string('foo'));
var_dump(coerce_to_string(42));
var_dump(coerce_to_string(3.14159));
var_dump(coerce_to_string(new Foo));
var_dump(coerce_to_string(false));
var_dump(coerce_to_string(true));

var_dump(coerce_to_string(new Bar));
var_dump(coerce_to_string(null));
var_dump(coerce_to_string([]));
var_dump(coerce_to_string($ref));

?>
--EXPECT--
string(3) "foo"
string(2) "42"
string(7) "3.14159"
string(3) "Bar"
string(0) ""
string(1) "1"
NULL
NULL
NULL
NULL
