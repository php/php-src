--TEST--
`never` parameter types - indicated via reflection
--FILE--
<?php

abstract class BoxedValue {
    abstract public function get(): mixed;
    abstract public function set(never $v): void;
}

$ref = new ReflectionParameter([BoxedValue::class, 'set'], 'v');
echo $ref . "\n";

$type = $ref->getType();

if (!($type instanceof ReflectionNamedType)) {
    throw new Exception("Should be named...");
}
echo "Name: " . $type->getName() . "\n";
echo "isBuiltin: " . $type->isBuiltIn() . "\n";
echo "allowsNull: " . (int)$type->allowsNull() . "\n";

?>
--EXPECT--
Parameter #0 [ <required> never $v ]
Name: never
isBuiltin: 1
allowsNull: 0
