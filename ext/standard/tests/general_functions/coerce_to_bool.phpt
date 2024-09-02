--TEST--
coerce_to_bool()
--FILE--
<?php

class Foo {}

$num = '42';
$ref = &$num;

var_dump(coerce_to_bool(42));
var_dump(coerce_to_bool(3.0));
var_dump(coerce_to_bool('42'));
var_dump(coerce_to_bool('42.0'));
var_dump(coerce_to_bool(true));
var_dump(coerce_to_bool($ref));
var_dump(coerce_to_bool(3.14159));
var_dump(coerce_to_bool('3.141'));
var_dump(coerce_to_bool(-1));
var_dump(coerce_to_bool('foo'));

var_dump(coerce_to_bool(false));
var_dump(coerce_to_bool(0));
var_dump(coerce_to_bool(0.0));

var_dump(coerce_to_bool(new Foo));
var_dump(coerce_to_bool(null));
var_dump(coerce_to_bool([]));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
NULL
NULL
NULL
