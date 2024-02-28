--TEST--
ReflectionTypes of relative class types (self, parent) from traits in classes, variadic parameter nullable types
--FILE--
<?php

trait TraitExample {
    public function bar(?parent ...$o) { return $o[0]; }
    public function ping(?self ...$o) { return $o[0]; }
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
        $parameters = $method->getParameters();
        foreach ($parameters as $param) {
            $type = $param->getType();
            echo "\t\tType: ", $type, PHP_EOL;
            echo "\t\tInstance of: ", $type::class, PHP_EOL;

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
		Type: ?B
		Instance of: ReflectionNamedType
				C::bar(): Argument #1 must be of type ?B, A given, called in %s on line %d
	Method: ping
		Type: ?C
		Instance of: ReflectionNamedType
				C::ping(): Argument #1 must be of type ?C, A given, called in %s on line %d
				C::ping(): Argument #1 must be of type ?C, B given, called in %s on line %d
Class: D
	Method: bar
		Type: ?B
		Instance of: ReflectionNamedType
				C::bar(): Argument #1 must be of type ?B, A given, called in %s on line %d
	Method: ping
		Type: ?C
		Instance of: ReflectionNamedType
				C::ping(): Argument #1 must be of type ?C, A given, called in %s on line %d
				C::ping(): Argument #1 must be of type ?C, B given, called in %s on line %d
