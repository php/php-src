--TEST--
ReflectionNamedType::getName() and ReflectionNamedType::__toString()
--FILE--
<?php

function testInternalTypes(?Traversable $traversable): ?string {
    return 'test';
}

function testUserDefinedTypes(?Test $traversable): ?Test {
    return new Test;
}

$function = new ReflectionFunction('testInternalTypes');
$type = $function->getParameters()[0]->getType();
$return = $function->getReturnType();

var_dump($type->getName());
var_dump((string) $type);
var_dump($return->getName());
var_dump((string) $return);

$function = new ReflectionFunction('testUserDefinedTypes');
$type = $function->getParameters()[0]->getType();
$return = $function->getReturnType();

var_dump($type->getName());
var_dump((string) $type);
var_dump($return->getName());
var_dump((string) $return);

?>
--EXPECT--
string(11) "Traversable"
string(12) "?Traversable"
string(6) "string"
string(7) "?string"
string(4) "Test"
string(5) "?Test"
string(4) "Test"
string(5) "?Test"
