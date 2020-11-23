--TEST--
Test ReflectionFunction::getClosure() function : basic functionality
--FILE--
<?php
echo "*** Testing ReflectionFunction::getClosure() : basic functionality ***\n";

function foo()
{
    var_dump( "Inside foo function" );
}

function bar( $arg )
{
    var_dump( "Arg is " . $arg );
}

$func = new ReflectionFunction( 'foo' );
$closure = $func->getClosure();
$closure();

$func = new ReflectionFunction( 'bar' );
$closure = $func->getClosure();
$closure( 'succeeded' );

?>
--EXPECT--
*** Testing ReflectionFunction::getClosure() : basic functionality ***
string(19) "Inside foo function"
string(16) "Arg is succeeded"
