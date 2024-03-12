--TEST--
ReflectionTypes of relative class type static in classes, return union types
--FILE--
<?php

class A {

}

class B extends A {
    public function foo($o): int|static { return $o; }
}

class C extends B {
    public function bar($o): int|static { return $o; }
}

class D extends C {}

$instances = [
    new A,
    new B,
    new C,
    new D,
];

foreach ($instances as $instance) {
    echo 'Class: ', $instance::class, PHP_EOL;
    $rc = new ReflectionClass($instance);
    $methods = $rc->getMethods();
    foreach ($methods as $method) {
        echo "\tMethod: ", $method->name, PHP_EOL;
        $unionType = $method->getReturnType();
        $types = $unionType->getTypes();
        foreach ($types as $type) {
            if (!($type instanceof ReflectionRelativeClassType)) {
                // Do not care about "int" type here;
                continue;
            }
            echo "\t\tType: ", $type, PHP_EOL;
            echo "\t\tInstance of: ", $type::class, PHP_EOL;
            $resolvedType = $type->resolveToNamedType();
            echo "\t\t\tResolved Type: ", $resolvedType, PHP_EOL;
            echo "\t\t\tInstance of: ", $resolvedType::class, PHP_EOL;

            foreach ($instances as $arg) {
                try {
                    $instance->{$method->name}($arg);
                } catch (\TypeError $e) {
                    echo "\t\t\t\t", $e->getMessage(), PHP_EOL;
                }
            }
        }
    }
}

?>
--EXPECTF--
Class: A
Class: B
	Method: foo
		Type: static
		Instance of: ReflectionRelativeClassType
			Resolved Type: B
			Instance of: ReflectionNamedType
				B::foo(): Return value must be of type B|int, A returned
Class: C
	Method: bar
		Type: static
		Instance of: ReflectionRelativeClassType
			Resolved Type: C
			Instance of: ReflectionNamedType
				C::bar(): Return value must be of type C|int, A returned
				C::bar(): Return value must be of type C|int, B returned
	Method: foo
		Type: static
		Instance of: ReflectionRelativeClassType
			Resolved Type: C
			Instance of: ReflectionNamedType
				B::foo(): Return value must be of type C|int, A returned
				B::foo(): Return value must be of type C|int, B returned
Class: D
	Method: bar
		Type: static
		Instance of: ReflectionRelativeClassType
			Resolved Type: D
			Instance of: ReflectionNamedType
				C::bar(): Return value must be of type D|int, A returned
				C::bar(): Return value must be of type D|int, B returned
				C::bar(): Return value must be of type D|int, C returned
	Method: foo
		Type: static
		Instance of: ReflectionRelativeClassType
			Resolved Type: D
			Instance of: ReflectionNamedType
				B::foo(): Return value must be of type D|int, A returned
				B::foo(): Return value must be of type D|int, B returned
				B::foo(): Return value must be of type D|int, C returned
