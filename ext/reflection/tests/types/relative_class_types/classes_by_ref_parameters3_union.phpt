--TEST--
ReflectionTypes of relative class types (self, parent) in classes, by-ref parameter union types
--FILE--
<?php

class A {

}

class B extends A {
    public function foo(int|self &$o) { return $o; }
}

class C extends B {
    public function bar(int|parent &$o) { return $o; }
    public function ping(int|self &$o) { return $o; }
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
        $parameters = $method->getParameters();
        foreach ($parameters as $param) {
            $unionType = $param->getType();
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
}

?>
--EXPECTF--
Class: A
Class: B
	Method: foo
		Type: self
		Instance of: ReflectionRelativeClassType
			Resolved Type: B
			Instance of: ReflectionNamedType
				B::foo(): Argument #1 ($o) must be of type B|int, A given, called in %s on line %d
Class: C
	Method: bar
		Type: parent
		Instance of: ReflectionRelativeClassType
			Resolved Type: B
			Instance of: ReflectionNamedType
				C::bar(): Argument #1 ($o) must be of type B|int, A given, called in %s on line %d
	Method: ping
		Type: self
		Instance of: ReflectionRelativeClassType
			Resolved Type: C
			Instance of: ReflectionNamedType
				C::ping(): Argument #1 ($o) must be of type C|int, A given, called in %s on line %d
				C::ping(): Argument #1 ($o) must be of type C|int, B given, called in %s on line %d
	Method: foo
		Type: self
		Instance of: ReflectionRelativeClassType
			Resolved Type: B
			Instance of: ReflectionNamedType
				B::foo(): Argument #1 ($o) must be of type B|int, A given, called in %s on line %d
Class: D
	Method: bar
		Type: parent
		Instance of: ReflectionRelativeClassType
			Resolved Type: B
			Instance of: ReflectionNamedType
				C::bar(): Argument #1 ($o) must be of type B|int, A given, called in %s on line %d
	Method: ping
		Type: self
		Instance of: ReflectionRelativeClassType
			Resolved Type: C
			Instance of: ReflectionNamedType
				C::ping(): Argument #1 ($o) must be of type C|int, A given, called in %s on line %d
				C::ping(): Argument #1 ($o) must be of type C|int, B given, called in %s on line %d
	Method: foo
		Type: self
		Instance of: ReflectionRelativeClassType
			Resolved Type: B
			Instance of: ReflectionNamedType
				B::foo(): Argument #1 ($o) must be of type B|int, A given, called in %s on line %d
