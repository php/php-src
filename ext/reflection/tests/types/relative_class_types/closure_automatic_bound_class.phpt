--TEST--
ReflectionTypes of relative class types (self, parent) in Closure bound to class automatically
--FILE--
<?php

class A {}
class B extends A {
    public function test() {
        $fnSelf = function (): self { };
        $fnParent = function (): parent {};
        $fnStatic = function (): static {};
        test_scopes($fnSelf, $fnParent, $fnStatic);
    }
}
class C extends B {}

function test_scopes(Closure ...$fns) {
    foreach ($fns as $fn) {
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

$b = new C();
$b->test();


?>
--EXPECT--
Bound to: C
Type: self
Instance of: ReflectionRelativeClassType
	Resolved Type: B
	Instance of: ReflectionNamedType
Bound to: C
Type: parent
Instance of: ReflectionRelativeClassType
	Resolved Type: A
	Instance of: ReflectionNamedType
Bound to: C
Type: static
Instance of: ReflectionRelativeClassType
	Resolved Type: C
	Instance of: ReflectionNamedType
