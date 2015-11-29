--TEST--
Method call on string literal
--FILE--
<?php
"string"->length();
?>
--EXPECTF--
Fatal error: Uncaught Error: Call to a member function length() on string in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
