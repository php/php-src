--TEST--
Using IAP functions on objects is deprecated
--FILE--
<?php

$obj = (object)['a' => 'b'];
var_dump(reset($obj));
var_dump(current($obj));
var_dump(key($obj));
var_dump(next($obj));
var_dump(end($obj));
var_dump(prev($obj));

?>
--EXPECTF--
Deprecated: reset(): Calling reset() on an object is deprecated in %s on line %d
string(1) "b"

Deprecated: current(): Calling current() on an object is deprecated in %s on line %d
string(1) "b"

Deprecated: key(): Calling key() on an object is deprecated in %s on line %d
string(1) "a"

Deprecated: next(): Calling next() on an object is deprecated in %s on line %d
bool(false)

Deprecated: end(): Calling end() on an object is deprecated in %s on line %d
string(1) "b"

Deprecated: prev(): Calling prev() on an object is deprecated in %s on line %d
bool(false)
