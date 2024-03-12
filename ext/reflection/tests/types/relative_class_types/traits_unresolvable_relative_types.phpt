--TEST--
ReflectionTypes of relative class types (self, parent) from traits in classes, variadic parameter DNF types
--FILE--
<?php

trait TraitExample {
    public function bar(object $o): parent { return $o; }
    public function ping(object $o): self { return $o; }
    public function pong(object $o): static { return $o; }
}

$rc = new ReflectionClass('TraitExample');

$methods = $rc->getMethods();
foreach ($methods as $method) {
    echo "Method: ", $method->name, PHP_EOL;
    $type = $method->getReturnType();
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
Method: bar
	Type: parent
	Instance of: ReflectionRelativeClassType
ReflectionException: Cannot resolve relative class name for a trait
Method: ping
	Type: self
	Instance of: ReflectionRelativeClassType
ReflectionException: Cannot resolve relative class name for a trait
Method: pong
	Type: static
	Instance of: ReflectionRelativeClassType
ReflectionException: Cannot resolve relative class name for a trait
