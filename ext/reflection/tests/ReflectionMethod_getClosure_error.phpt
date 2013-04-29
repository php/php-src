--TEST--
Test ReflectionMethod::getClosure() function : error functionality
--FILE--
<?php
/* Prototype  : public mixed ReflectionFunction::getClosure()
 * Description: Returns a dynamically created closure for the method
 * Source code: ext/reflection/php_reflection.c
 * Alias to functions:
 */

echo "*** Testing ReflectionMethod::getClosure() : error conditions ***\n";

class StaticExample
{
	static function foo()
	{
		var_dump( "Static Example class, Hello World!" );
	}
}

class Example
{
	public $bar = 42;
	public function foo()
	{
		var_dump( "Example class, bar: " . $this->bar );
	}
}

// Initialize classes
$class = new ReflectionClass( 'Example' );
$staticclass = new ReflectionClass( 'StaticExample' );
$method = $class->getMethod( 'foo' );
$staticmethod = $staticclass->getMethod( 'foo' );
$object = new Example();
$fakeobj = new StdClass();

echo "\n-- Testing ReflectionMethod::getClosure() function with more than expected no. of arguments --\n";
var_dump( $staticmethod->getClosure( 'foobar' ) );
var_dump( $staticmethod->getClosure( 'foo', 'bar' ) );
var_dump( $method->getClosure( $object, 'foobar' ) );

echo "\n-- Testing ReflectionMethod::getClosure() function with Zero arguments --\n";
$closure = $method->getClosure();

echo "\n-- Testing ReflectionMethod::getClosure() function with Zero arguments --\n";
try {
        var_dump( $method->getClosure( $fakeobj ) );
} catch( Exception $e ) {
        var_dump( $e->getMessage() );
}

?>
===DONE===
--EXPECTF--
*** Testing ReflectionMethod::getClosure() : error conditions ***

-- Testing ReflectionMethod::getClosure() function with more than expected no. of arguments --
object(Closure)#%d (0) {
}
object(Closure)#%d (0) {
}

Warning: ReflectionMethod::getClosure() expects exactly 1 parameter, 2 given in %s on line %d
NULL

-- Testing ReflectionMethod::getClosure() function with Zero arguments --

Warning: ReflectionMethod::getClosure() expects exactly 1 parameter, 0 given in %s on line %d

-- Testing ReflectionMethod::getClosure() function with Zero arguments --
string(72) "Given object is not an instance of the class this method was declared in"
===DONE===
