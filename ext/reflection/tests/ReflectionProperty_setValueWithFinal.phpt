--TEST--
ReflectionClass::setValue() with final properties
--FILE--
<?php
class Foo {
    final public string $a;
    final public string $b = "foo";
}

$foo = new Foo();

$fooReflection = new ReflectionProperty(Foo::class, "a");
$fooReflection->setValue($foo, "bar");
var_dump($foo->a);

$fooReflection = new ReflectionProperty(Foo::class, "b");
try {
    $fooReflection->setValue($foo, "bar");
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
string(3) "bar"
Cannot modify final property Foo::$b after initialization
