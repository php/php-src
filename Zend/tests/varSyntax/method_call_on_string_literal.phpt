--TEST--
Method call on string literal
--FILE--
<?php
"string"->length();
?>
--EXPECTF--
Fatal error: Call to a member function length() on string in %s on line %d
