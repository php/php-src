--TEST--
GH-16905 (Internal iterator functions can't handle UNDEF properties)
--FILE--
<?php

class TestSomeUndef {
	public int $a;
	public int $b;
	public int $c;
	public int $d;
}

class TestAllUndef {
	public int $a;
}

$x = new TestSomeUndef;
$x->b = 1;
$x->c = 2;

var_dump(reset($x));
var_dump(current($x));
var_dump(end($x));

var_dump(reset($x));
var_dump(next($x));

var_dump(end($x));
var_dump(prev($x));

var_dump(key($x));
var_dump(current($x));

$x = new TestAllUndef;
var_dump(key($x));
var_dump(current($x));

$x->a = 1;
var_dump(key($x));
var_dump(current($x));
reset($x);
var_dump(key($x));
var_dump(current($x));

?>
--EXPECTF--
Deprecated: reset(): Calling reset() on an object is deprecated in %s on line %d
int(1)

Deprecated: current(): Calling current() on an object is deprecated in %s on line %d
int(1)

Deprecated: end(): Calling end() on an object is deprecated in %s on line %d
int(2)

Deprecated: reset(): Calling reset() on an object is deprecated in %s on line %d
int(1)

Deprecated: next(): Calling next() on an object is deprecated in %s on line %d
int(2)

Deprecated: end(): Calling end() on an object is deprecated in %s on line %d
int(2)

Deprecated: prev(): Calling prev() on an object is deprecated in %s on line %d
int(1)

Deprecated: key(): Calling key() on an object is deprecated in %s on line %d
string(1) "b"

Deprecated: current(): Calling current() on an object is deprecated in %s on line %d
int(1)

Deprecated: key(): Calling key() on an object is deprecated in %s on line %d
NULL

Deprecated: current(): Calling current() on an object is deprecated in %s on line %d
bool(false)

Deprecated: key(): Calling key() on an object is deprecated in %s on line %d
NULL

Deprecated: current(): Calling current() on an object is deprecated in %s on line %d
bool(false)

Deprecated: reset(): Calling reset() on an object is deprecated in %s on line %d

Deprecated: key(): Calling key() on an object is deprecated in %s on line %d
string(1) "a"

Deprecated: current(): Calling current() on an object is deprecated in %s on line %d
int(1)
