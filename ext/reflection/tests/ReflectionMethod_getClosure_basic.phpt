--TEST--
Test ReflectionMethod::getClosure() function : basic functionality
--FILE--
<?php
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
--EXPECT--
*** Testing ReflectionMethod::getClosure() : basic functionality ***
string(34) "Static Example class, Hello World!"
string(22) "Example class, bar: 42"
string(22) "Example class, bar: 34"
