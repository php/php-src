--TEST--
Scalar methods: length() returns int and is not treated as a chainable string
--FILE--
<?php
// length() desugars to Str::length() and returns an int.
var_dump("hello"->length());

// Because length() returns int (not string), it is NOT a chainable string
// receiver: the OUTER call is left as a normal method call on an int, which
// fails at runtime with "member function on int" -- proving the outer call was
// not desugared to a Str:: dispatch.
"hi"->length()->upper();
?>
--EXPECTF--
int(5)

Fatal error: Uncaught Error: Call to a member function upper() on int in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
