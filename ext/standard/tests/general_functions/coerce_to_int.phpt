--TEST--
coerce_to_int()
--FILE--
<?php

class Foo {}

$num = '42';
$ref = &$num;

var_dump(coerce_to_int(42));
var_dump(coerce_to_int(3.0));
var_dump(coerce_to_int('42'));
var_dump(coerce_to_int('42.0'));
var_dump(coerce_to_int(false));
var_dump(coerce_to_int(true));
var_dump(coerce_to_int($ref));

var_dump(coerce_to_int(3.14159));
var_dump(coerce_to_int('3.141'));

var_dump(coerce_to_int('foo'));
var_dump(coerce_to_int(new Foo));
var_dump(coerce_to_int(null));
var_dump(coerce_to_int([]));

?>
--EXPECTF--
int(42)
int(3)
int(42)
int(42)
int(0)
int(1)
int(42)

Deprecated: Implicit conversion from float 3.14159 to int loses precision in %s on line %d
int(3)

Deprecated: Implicit conversion from float-string "3.141" to int loses precision in %s on line %d
int(3)
NULL
NULL
NULL
NULL
