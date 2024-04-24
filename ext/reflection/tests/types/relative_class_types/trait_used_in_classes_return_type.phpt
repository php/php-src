--TEST--
ReflectionTypes of relative class types (self, parent, static) from traits in classes, return types
--FILE--
<?php

trait TraitExample {
    public function bar(object $o): parent { return $o; }
    public function ping(object $o): self { return $o; }
    public function pong(object $o): static { return $o; }
}

class A {

}

class B extends A {
}

class C extends B {
    use TraitExample;
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
        $type = $method->getReturnType();
        echo "\t\tType: ", $type, PHP_EOL;
        echo "\t\tInstance of: ", $type::class, PHP_EOL;
        if ($type instanceof ReflectionRelativeClassType) {
            $resolvedType = $type->resolveToNamedType();
            echo "\t\t\tResolved Type: ", $resolvedType, PHP_EOL;
            echo "\t\t\tInstance of: ", $resolvedType::class, PHP_EOL;
        }

        foreach ($instances as $arg) {
            try {
                $instance->{$method->name}($arg);
            } catch (\TypeError $e) {
                echo "\t\t\t\t", $e->getMessage(), PHP_EOL;
            }
        }
    }
}

?>
--EXPECT--
Class: A
Class: B
Class: C
	Method: bar
		Type: B
		Instance of: ReflectionNamedType
				C::bar(): Return value must be of type B, A returned
	Method: ping
		Type: C
		Instance of: ReflectionNamedType
				C::ping(): Return value must be of type C, A returned
				C::ping(): Return value must be of type C, B returned
	Method: pong
		Type: static
		Instance of: ReflectionRelativeClassType
			Resolved Type: C
			Instance of: ReflectionNamedType
				C::pong(): Return value must be of type C, A returned
				C::pong(): Return value must be of type C, B returned
Class: D
	Method: bar
		Type: B
		Instance of: ReflectionNamedType
				C::bar(): Return value must be of type B, A returned
	Method: ping
		Type: C
		Instance of: ReflectionNamedType
				C::ping(): Return value must be of type C, A returned
				C::ping(): Return value must be of type C, B returned
	Method: pong
		Type: static
		Instance of: ReflectionRelativeClassType
			Resolved Type: D
			Instance of: ReflectionNamedType
				C::pong(): Return value must be of type D, A returned
				C::pong(): Return value must be of type D, B returned
				C::pong(): Return value must be of type D, C returned
