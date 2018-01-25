--TEST--
Test ReflectionMethod::getClosure() function : basic functionality
--FILE--
<?php
/* Prototype  : public mixed ReflectionFunction::getClosure()
 * Description: Returns a dynamically created closure for the method
 * Source code: ext/reflection/php_reflection.c
 * Alias to functions:
 */

echo "*** Testing ReflectionMethod::getClosure() : basic functionality ***\n";

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
$object = new Example();
$fakeobj = new StdClass();


$method = $staticclass->getMethod( 'foo' );
$closure = $method->getClosure();
$closure();

$method = $class->getMethod( 'foo' );

$closure = $method->getClosure( $object );
$closure();
$object->bar = 34;
$closure();

?>
===DONE===
--EXPECTF--
*** Testing ReflectionMethod::getClosure() : basic functionality ***
string(34) "Static Example class, Hello World!"
string(22) "Example class, bar: 42"
string(22) "Example class, bar: 34"
===DONE===
