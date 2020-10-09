--TEST--
Reflection::getClosureThis()
--FILE--
<?php
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

$method = $staticclass->getMethod( 'foo' );
$closure = $method->getClosure();
$rf = new ReflectionFunction($closure);

var_dump($rf->getClosureThis());

$method = $class->getMethod( 'foo' );

$closure = $method->getClosure( $object );
$rf = new ReflectionFunction($closure);

var_dump($rf->getClosureThis());

echo "Done!\n";
?>
--EXPECTF--
NULL
object(Example)#%d (1) {
  ["bar"]=>
  int(42)
}
Done!
