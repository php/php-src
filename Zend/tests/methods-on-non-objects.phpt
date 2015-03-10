--TEST--
Method calls on non-objects raise recoverable errors
--FILE--
<?php

$x= null;
$x->method();
echo "Should not get here!\n";
?>
--EXPECTF--

Fatal error: Call to a member function method() on null in %s on line %d 
