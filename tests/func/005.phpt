--TEST--
Testing register_shutdown_function()
--FILE--
<?php 

function foo()
{
	print "foo";
}

register_shutdown_function("foo");

print "foo() will be called on shutdown...\n";

?>
--EXPECT--
foo() will be called on shutdown...
foo

