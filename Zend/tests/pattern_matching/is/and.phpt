--TEST--
And pattern
--FILE--
<?php

interface A {}
interface B {}
interface C {}
class Foo implements A, B {}

var_dump(1 is int & 1);
var_dump(2 is int & (1|2));
var_dump(3 is float & 1);
var_dump(4 is int & float);
var_dump([] is [] & [...]);
var_dump('foo' is string & 'bar');
var_dump(new Foo() is A&B);
var_dump(new Foo() is (A&C));

?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
