--TEST--
ReflectionTypes of relative class types (self, parent, static) in interfaces
--FILE--
<?php

interface A {

}

interface B extends A {
    public function foo(object $o): self;
}

interface C extends B {
    //public function bar(object $o): parent; // This compile errors
    public function ping(object $o): self;
    public function pong(object $o): static;
}

interface D extends C {}

$interfaces = [
    'A',
    'B',
    'C',
    'D',
];

foreach ($interfaces as $interface) {
    echo 'Interface: ', $interface, PHP_EOL;
    $rc = new ReflectionClass($interface);
    $methods = $rc->getMethods();
    foreach ($methods as $method) {
        echo "\tMethod: ", $method->name, PHP_EOL;
        $type = $method->getReturnType();
        echo "\t\tType: ", $type, PHP_EOL;
        echo "\t\tInstance of: ", $type::class, PHP_EOL;
        try {
            $resolvedType = $type->resolveToNamedType();
            echo "\t\t\tResolved Type: ", $resolvedType, PHP_EOL;
            echo "\t\t\tInstance of: ", $resolvedType::class, PHP_EOL;
        } catch (ReflectionException $e) {
            echo $e->getMessage(), PHP_EOL;
        }
    }
}

?>
--EXPECT--
Interface: A
Interface: B
	Method: foo
		Type: self
		Instance of: ReflectionRelativeClassType
			Resolved Type: B
			Instance of: ReflectionNamedType
Interface: C
	Method: ping
		Type: self
		Instance of: ReflectionRelativeClassType
			Resolved Type: C
			Instance of: ReflectionNamedType
	Method: pong
		Type: static
		Instance of: ReflectionRelativeClassType
Cannot resolve "static" type of an interface
	Method: foo
		Type: self
		Instance of: ReflectionRelativeClassType
			Resolved Type: B
			Instance of: ReflectionNamedType
Interface: D
	Method: ping
		Type: self
		Instance of: ReflectionRelativeClassType
			Resolved Type: C
			Instance of: ReflectionNamedType
	Method: pong
		Type: static
		Instance of: ReflectionRelativeClassType
Cannot resolve "static" type of an interface
	Method: foo
		Type: self
		Instance of: ReflectionRelativeClassType
			Resolved Type: B
			Instance of: ReflectionNamedType
