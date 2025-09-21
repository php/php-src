--TEST--
ReflectionClass::newInstanceFromData - bad instantiations
--FILE--
<?php

// internal classes
$rcDateTime = new ReflectionClass('DateTime'); // with constructor
$rcPDOStatement = new ReflectionClass('PDOStatement'); // no constructor

// not classes
$rcStringable = new ReflectionClass('Stringable');

interface MyInterface {}
$rcMyInterface = new ReflectionClass('MyInterface');

trait MyTrait {}
$rcMyTrait = new ReflectionClass('MyTrait');

// class with private constructor
class A
{
    public int $a;

    private function __construct()
    {
        echo "In constructor of class A\n";
    }
}

$rcA = new ReflectionClass('A');

try
{
    $rcDateTime->newInstanceFromData([], ['now', new DateTimeZone('UTC')]);
}
catch(Throwable $e)
{
    echo "Exception: " . $e->getMessage() . "\n";
}

try
{
    $rcPDOStatement->newInstanceFromData(['a' => 123]);
}
catch(Throwable $e)
{
    echo "Exception: " . $e->getMessage() . "\n";
}

try
{
    $rcMyInterface->newInstanceFromData(['a' => 123]);
}
catch(Throwable $e)
{
    echo "Exception: " . $e->getMessage() . "\n";
}

try
{
    $rcMyTrait->newInstanceFromData(['a' => 123]);
}
catch(Throwable $e)
{
    echo "Exception: " . $e->getMessage() . "\n";
}

try
{
    $rcA->newInstanceFromData(['a' => 123]);
}
catch(Throwable $e)
{
    echo "Exception: " . $e->getMessage() . "\n";
}

?>
--EXPECTF--
Exception: Class DateTime is an internal class that cannot be instantiated from data
Exception: Class PDOStatement is an internal class that cannot be instantiated from data
Exception: Cannot instantiate interface MyInterface
Exception: Cannot instantiate trait MyTrait
Exception: Access to non-public constructor of class A
