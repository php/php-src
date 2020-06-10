--TEST--
BackedEnum::tryFrom() casing in reflection
--FILE--
<?php

enum Foo: string {}

$reflectionEnum = new ReflectionEnum(Foo::class);
$reflectionMethod = $reflectionEnum->getMethod('tryFrom');

echo $reflectionMethod->getName() . "\n";

?>
--EXPECT--
tryFrom
