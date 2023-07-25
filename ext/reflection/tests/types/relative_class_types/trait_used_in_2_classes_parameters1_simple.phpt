--TEST--
ReflectionTypes of relative class types (self, parent) from traits in 2 different unrelated classes, parameter types
--FILE--
<?php

trait TraitExample {
    public function bar(parent $o) { return $o; }
    public function ping(self $o) { return $o; }
    //public function barNull(?parent $o) { return $o; }
    //public function pingNull(?self $o) { return $o; }
}

class A {

}

class B extends A {
}

class C extends B {
    use TraitExample;
}

class D extends C {}

class A2 {

}

class B2 extends A2 {
}

class C2 extends B2 {
    use TraitExample;
}

class D2 extends C2 {}

$instances = [
    new A,
    new B,
    new C,
    new D,
    new A2,
    new B2,
    new C2,
    new D2,
];

foreach ($instances as $instance) {
    echo 'Class: ', $instance::class, PHP_EOL;
    $rc = new ReflectionClass($instance);
    $methods = $rc->getMethods();
    foreach ($methods as $method) {
        echo "\tMethod: ", $method->name, PHP_EOL;
        $parameters = $method->getParameters();
        foreach ($parameters as $param) {
            $type = $param->getType();
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
Class: C
	Method: bar
		Type: parent
		Instance of: ReflectionRelativeClassType
			Resolved Type: B
			Instance of: ReflectionNamedType
				C::bar(): Argument #1 ($o) must be of type B, A given, called in %s on line %d
				C::bar(): Argument #1 ($o) must be of type B, A2 given, called in %s on line %d
				C::bar(): Argument #1 ($o) must be of type B, B2 given, called in %s on line %d
				C::bar(): Argument #1 ($o) must be of type B, C2 given, called in %s on line %d
				C::bar(): Argument #1 ($o) must be of type B, D2 given, called in %s on line %d
	Method: ping
		Type: self
		Instance of: ReflectionRelativeClassType
			Resolved Type: C
			Instance of: ReflectionNamedType
				C::ping(): Argument #1 ($o) must be of type C, A given, called in %s on line %d
				C::ping(): Argument #1 ($o) must be of type C, B given, called in %s on line %d
				C::ping(): Argument #1 ($o) must be of type C, A2 given, called in %s on line %d
				C::ping(): Argument #1 ($o) must be of type C, B2 given, called in %s on line %d
				C::ping(): Argument #1 ($o) must be of type C, C2 given, called in %s on line %d
				C::ping(): Argument #1 ($o) must be of type C, D2 given, called in %s on line %d
Class: D
	Method: bar
		Type: parent
		Instance of: ReflectionRelativeClassType
			Resolved Type: B
			Instance of: ReflectionNamedType
				C::bar(): Argument #1 ($o) must be of type B, A given, called in %s on line %d
				C::bar(): Argument #1 ($o) must be of type B, A2 given, called in %s on line %d
				C::bar(): Argument #1 ($o) must be of type B, B2 given, called in %s on line %d
				C::bar(): Argument #1 ($o) must be of type B, C2 given, called in %s on line %d
				C::bar(): Argument #1 ($o) must be of type B, D2 given, called in %s on line %d
	Method: ping
		Type: self
		Instance of: ReflectionRelativeClassType
			Resolved Type: C
			Instance of: ReflectionNamedType
				C::ping(): Argument #1 ($o) must be of type C, A given, called in %s on line %d
				C::ping(): Argument #1 ($o) must be of type C, B given, called in %s on line %d
				C::ping(): Argument #1 ($o) must be of type C, A2 given, called in %s on line %d
				C::ping(): Argument #1 ($o) must be of type C, B2 given, called in %s on line %d
				C::ping(): Argument #1 ($o) must be of type C, C2 given, called in %s on line %d
				C::ping(): Argument #1 ($o) must be of type C, D2 given, called in %s on line %d
Class: A2
Class: B2
Class: C2
	Method: bar
		Type: parent
		Instance of: ReflectionRelativeClassType
			Resolved Type: B2
			Instance of: ReflectionNamedType
				C2::bar(): Argument #1 ($o) must be of type B2, A given, called in %s on line %d
				C2::bar(): Argument #1 ($o) must be of type B2, B given, called in %s on line %d
				C2::bar(): Argument #1 ($o) must be of type B2, C given, called in %s on line %d
				C2::bar(): Argument #1 ($o) must be of type B2, D given, called in %s on line %d
				C2::bar(): Argument #1 ($o) must be of type B2, A2 given, called in %s on line %d
	Method: ping
		Type: self
		Instance of: ReflectionRelativeClassType
			Resolved Type: C2
			Instance of: ReflectionNamedType
				C2::ping(): Argument #1 ($o) must be of type C2, A given, called in %s on line %d
				C2::ping(): Argument #1 ($o) must be of type C2, B given, called in %s on line %d
				C2::ping(): Argument #1 ($o) must be of type C2, C given, called in %s on line %d
				C2::ping(): Argument #1 ($o) must be of type C2, D given, called in %s on line %d
				C2::ping(): Argument #1 ($o) must be of type C2, A2 given, called in %s on line %d
				C2::ping(): Argument #1 ($o) must be of type C2, B2 given, called in %s on line %d
Class: D2
	Method: bar
		Type: parent
		Instance of: ReflectionRelativeClassType
			Resolved Type: B2
			Instance of: ReflectionNamedType
				C2::bar(): Argument #1 ($o) must be of type B2, A given, called in %s on line %d
				C2::bar(): Argument #1 ($o) must be of type B2, B given, called in %s on line %d
				C2::bar(): Argument #1 ($o) must be of type B2, C given, called in %s on line %d
				C2::bar(): Argument #1 ($o) must be of type B2, D given, called in %s on line %d
				C2::bar(): Argument #1 ($o) must be of type B2, A2 given, called in %s on line %d
	Method: ping
		Type: self
		Instance of: ReflectionRelativeClassType
			Resolved Type: C2
			Instance of: ReflectionNamedType
				C2::ping(): Argument #1 ($o) must be of type C2, A given, called in %s on line %d
				C2::ping(): Argument #1 ($o) must be of type C2, B given, called in %s on line %d
				C2::ping(): Argument #1 ($o) must be of type C2, C given, called in %s on line %d
				C2::ping(): Argument #1 ($o) must be of type C2, D given, called in %s on line %d
				C2::ping(): Argument #1 ($o) must be of type C2, A2 given, called in %s on line %d
				C2::ping(): Argument #1 ($o) must be of type C2, B2 given, called in %s on line %d
