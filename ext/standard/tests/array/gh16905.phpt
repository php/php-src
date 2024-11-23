--TEST--
GH-16905 (Internal iterator functions can't handle UNDEF properties)
--FILE--
<?php

class X {
	public int $a;
}

$x = new X;
var_dump(reset($x));
var_dump(current($x));
var_dump(end($x));
var_dump(next($x));
var_dump(prev($x));
var_dump(value: key($x));

?>
--EXPECTF--
Deprecated: reset(): Calling reset() on an object is deprecated in %s on line %d
bool(false)

Deprecated: current(): Calling current() on an object is deprecated in %s on line %d
bool(false)

Deprecated: end(): Calling end() on an object is deprecated in %s on line %d
bool(false)

Deprecated: next(): Calling next() on an object is deprecated in %s on line %d
bool(false)

Deprecated: prev(): Calling prev() on an object is deprecated in %s on line %d
bool(false)

Deprecated: key(): Calling key() on an object is deprecated in %s on line %d
NULL
