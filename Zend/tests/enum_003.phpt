--TEST--
Enum inheritance checks
--FILE--
<?php

enum a { b }
enum c { d }

var_dump(a::b instanceof a);
var_dump(a::b instanceof c);
var_dump(a::b instanceof stdClass);

function foo(a $a) { var_dump($a); }
foo(a::b);
foo(c::d);

?>
--EXPECTF--
bool(true)
bool(false)
bool(false)
enum(a::b)

Fatal error: Uncaught TypeError: Argument 1 passed to foo() must be an instance of a, instance of c given, called in %s on line %d and defined in %s:%d
Stack trace:
#0 %s(%d): foo(c::d)
#1 {main}
  thrown in %s on line %d
