--TEST--
Test ReflectionFunction::getClosure() function : error functionality
--FILE--
<?php
/* Prototype  : public mixed ReflectionFunction::getClosure()
 * Description: Returns a dynamically created closure for the function
 * Source code: ext/reflection/php_reflection.c
 * Alias to functions:
 */

echo "*** Testing ReflectionFunction::getClosure() : error conditions ***\n";

function foo()
{
	var_dump( "Inside foo function" );
}

$func = new ReflectionFunction( 'foo' );
$closure = $func->getClosure('bar');

?>
===DONE===
--EXPECTF--
*** Testing ReflectionFunction::getClosure() : error conditions ***

Warning: Wrong parameter count for ReflectionFunction::getClosure() in %s on line %d
===DONE===
