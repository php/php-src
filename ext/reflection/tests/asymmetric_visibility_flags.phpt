--TEST--
ReflectionProperty::is{Private,Protected}Set
--FILE--
<?php

class Foo {
    public private(set) int $bar;
    public protected(set) int $baz;
}

function test($property) {
  $reflectionProperty = new ReflectionProperty(Foo::class, $property);
  var_dump($reflectionProperty->isPrivateSet());
  var_dump($reflectionProperty->isProtectedSet());
  var_dump(($reflectionProperty->getModifiers() & ReflectionProperty::IS_PRIVATE_SET) !== 0);
  var_dump(($reflectionProperty->getModifiers() & ReflectionProperty::IS_PROTECTED_SET) !== 0);
  echo $reflectionProperty;
}

test('bar');
test('baz');

?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
Property [ public private(set) int $bar ]
bool(false)
bool(true)
bool(false)
bool(true)
Property [ public protected(set) int $baz ]
