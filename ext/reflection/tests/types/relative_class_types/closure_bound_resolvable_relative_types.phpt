--TEST--
ReflectionTypes of relative class types (self, parent) from traits in classes, variadic parameter DNF types
--XFAIL--
I'm confused why it not possible to chope the bound scope via Reflection
--FILE--
<?php

class A {}
class B extends A {}

$fnSelf = function (): self { };
$fnParent = function (): parent {};
$fnStatic = function (): static {};

$instances = [
    $fnSelf,
    $fnParent,
    $fnStatic,
];

$b = new B();

// TODO Method getClosureScopeClass() seems useful
// TODO Need to pass scope to: ZEND_METHOD(ReflectionFunctionAbstract, getParameters) ?
// TODO Need to pass scope to: ZEND_METHOD(ReflectionFunctionAbstract, getReturnType) ?
foreach ($instances as $instance) {
    $instance->bindTo($b);
    $rc = new ReflectionFunction($instance);
    echo "\tBound to: ", $rc->getClosureScopeClass()->name, PHP_EOL;
    $type = $rc->getReturnType();
    echo "\tType: ", $type, PHP_EOL;
    echo "\tInstance of: ", $type::class, PHP_EOL;
    try {
        $resolvedType = $type->resolveToNamedType();
        echo "\t\tResolved Type: ", $resolvedType, PHP_EOL;
        echo "\t\tInstance of: ", $resolvedType::class, PHP_EOL;
    } catch (\Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

?>
--EXPECT--
