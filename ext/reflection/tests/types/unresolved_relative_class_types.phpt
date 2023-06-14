--TEST--
ReflectionTypes of unresolved relative class types (self, parent, static)
--FILE--
<?php

/* An unresolved relative class type can only happen in unbound closures and traits */
$fn1 = function($x): self {};
$fn2 = function($x): parent {};
$fn3 = function($x): static {};

$closures = [
    $fn1,
    $fn2,
    $fn3,
];
foreach ($closures as $closure) {
    $rf = new ReflectionFunction($closure);
    echo "Closure:", PHP_EOL;
    $type = $rf->getReturnType();
    echo "\tType: ", $type, PHP_EOL;
    echo "\tInstance of: ", $type::class, PHP_EOL;
    try {
        $resolvedType = $type->resolveToNamedType();
        echo "\t\tResolved Type: ", $resolvedType, PHP_EOL;
        echo "\t\tInstance of: ", $resolvedType::class, PHP_EOL;
    } catch (ReflectionException $e) {
        echo $e->getMessage(), PHP_EOL;
    }
}

echo PHP_EOL, "Trait:", PHP_EOL;
trait A {
    public function bar(object $o): parent { return $o; }
    public function ping(object $o): self { return $o; }
    public function pong(object $o): static { return $o; }
}

$rc = new ReflectionClass('A');
$methods = $rc->getMethods();
foreach ($methods as $method) {
    echo "Method: ", $method->name, PHP_EOL;
    $type = $method->getReturnType();
    echo "\tType: ", $type, PHP_EOL;
    echo "\tInstance of: ", $type::class, PHP_EOL;
    try {
        $resolvedType = $type->resolveToNamedType();
        echo "\t\tResolved Type: ", $resolvedType, PHP_EOL;
        echo "\t\tInstance of: ", $resolvedType::class, PHP_EOL;
    } catch (ReflectionException $e) {
        echo $e->getMessage(), PHP_EOL;
    }
}

?>
--EXPECT--
Closure:
	Type: self
	Instance of: ReflectionRelativeClassType
Cannot resolve relative class name for a closure
Closure:
	Type: parent
	Instance of: ReflectionRelativeClassType
Cannot resolve relative class name for a closure
Closure:
	Type: static
	Instance of: ReflectionRelativeClassType
Cannot resolve relative class name for a closure

Trait:
Method: bar
	Type: parent
	Instance of: ReflectionRelativeClassType
Cannot resolve relative class name for a trait
Method: ping
	Type: self
	Instance of: ReflectionRelativeClassType
Cannot resolve relative class name for a trait
Method: pong
	Type: static
	Instance of: ReflectionRelativeClassType
Cannot resolve relative class name for a trait
