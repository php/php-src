--TEST--
Method call on string literal dispatches to the Str backing class (scalar methods)
--FILE--
<?php
var_dump("  string  "->trim());
var_dump("string"->length());
var_dump("ABC"->lower());
// An unknown method errors as an undefined method on Str, not "member function on string".
"string"->noSuchMethod();
?>
--EXPECTF--
string(6) "string"
int(6)
string(3) "abc"

Fatal error: Uncaught Error: Call to undefined method Str::noSuchMethod() in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
