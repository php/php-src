--TEST--
ReflectionTypes of relative class type parent in Closure bound to class with no parent
--FILE--
<?php

class A {}

$fnParent = function (): parent {};

$a = new A();

$fn = $fnParent->bindTo($a);
$rc = new ReflectionFunction($fn);
echo "Bound to: ", $rc->getClosureCalledClass()->name, PHP_EOL;
$type = $rc->getReturnType();
echo "Type: ", $type, PHP_EOL;
echo "Instance of: ", $type::class, PHP_EOL;
try {
    $resolvedType = $type->resolveToNamedType();
    echo "\tResolved Type: ", $resolvedType, PHP_EOL;
    echo "\tInstance of: ", $resolvedType::class, PHP_EOL;
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Bound to: A
Type: parent
Instance of: ReflectionRelativeClassType
ReflectionException: Cannot resolve "parent" type when class has no parent
