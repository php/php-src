--TEST--
Method calls on non-objects raise recoverable errors
--FILE--
<?php

$x= null;
$x->method();
echo "Should not get here!\n";
?>
--EXPECTF--

Fatal error: Uncaught Error: Call to a member function method() on null in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d 
