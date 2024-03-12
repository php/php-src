--TEST--
ReflectionTypes of relative class types (self, parent) from traits in classes, variadic parameter DNF types
--FILE--
<?php

$fnSelf = function (): self { };
$fnParent = function (): parent {};
$fnStatic = function (): static {};

$instances = [
    $fnSelf,
    $fnParent,
    $fnStatic,
];

foreach ($instances as $instance) {
    $rc = new ReflectionFunction($instance);
    $type = $rc->getReturnType();
    echo "\t\tType: ", $type, PHP_EOL;
    echo "\t\tInstance of: ", $type::class, PHP_EOL;
    try {
        $resolvedType = $type->resolveToNamedType();
        echo "\t\t\tResolved Type: ", $resolvedType, PHP_EOL;
        echo "\t\t\tInstance of: ", $resolvedType::class, PHP_EOL;
    } catch (\Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

?>
--EXPECT--
Type: self
		Instance of: ReflectionRelativeClassType
ReflectionException: Cannot resolve relative class name for a closure
		Type: parent
		Instance of: ReflectionRelativeClassType
ReflectionException: Cannot resolve relative class name for a closure
		Type: static
		Instance of: ReflectionRelativeClassType
ReflectionException: Cannot resolve relative class name for a closure
