--TEST--
coerce_to_float()
--FILE--
<?php

class Foo {}

$num = '42';
$ref = &$num;

var_dump(coerce_to_float(42));
var_dump(coerce_to_float(3.0));
var_dump(coerce_to_float('42'));
var_dump(coerce_to_float('42.0'));
var_dump(coerce_to_float(false));
var_dump(coerce_to_float(true));
var_dump(coerce_to_float($ref));
var_dump(coerce_to_float(3.14159));
var_dump(coerce_to_float('3.141'));

var_dump(coerce_to_float('foo'));
var_dump(coerce_to_float(new Foo));
var_dump(coerce_to_float(null));
var_dump(coerce_to_float([]));

?>
--EXPECT--
float(42)
float(3)
float(42)
float(42)
float(0)
float(1)
float(42)
float(3.14159)
float(3.141)
NULL
NULL
NULL
NULL
