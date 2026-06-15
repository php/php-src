--TEST--
Scalar methods: unknown method on a string literal errors as undefined Str method
--FILE--
<?php
"x"->nope();
?>
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined method Str::nope() in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
