--TEST--
ReflectionTypes of relative class types (self, parent) in Closure bound to class with parent
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
$scopes = [
    'static',
    B::class,
    A::class,
];

$b = new B();

foreach ($scopes as $scope) {
    echo "Scope as \"$scope\"", PHP_EOL;
    foreach ($instances as $instance) {
        $fn = $instance->bindTo($b, $scope);
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
    }
}

?>
--EXPECT--
Scope as "static"
Bound to: B
Type: self
Instance of: ReflectionRelativeClassType
	Resolved Type: Closure
	Instance of: ReflectionNamedType
Bound to: B
Type: parent
Instance of: ReflectionRelativeClassType
ReflectionException: Cannot resolve "parent" type when class has no parent
Bound to: B
Type: static
Instance of: ReflectionRelativeClassType
	Resolved Type: B
	Instance of: ReflectionNamedType
Scope as "B"
Bound to: B
Type: self
Instance of: ReflectionRelativeClassType
	Resolved Type: B
	Instance of: ReflectionNamedType
Bound to: B
Type: parent
Instance of: ReflectionRelativeClassType
	Resolved Type: A
	Instance of: ReflectionNamedType
Bound to: B
Type: static
Instance of: ReflectionRelativeClassType
	Resolved Type: B
	Instance of: ReflectionNamedType
Scope as "A"
Bound to: B
Type: self
Instance of: ReflectionRelativeClassType
	Resolved Type: A
	Instance of: ReflectionNamedType
Bound to: B
Type: parent
Instance of: ReflectionRelativeClassType
ReflectionException: Cannot resolve "parent" type when class has no parent
Bound to: B
Type: static
Instance of: ReflectionRelativeClassType
	Resolved Type: B
	Instance of: ReflectionNamedType
