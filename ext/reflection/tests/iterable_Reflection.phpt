--TEST--
iterable Type in Reflection
--FILE--
<?php

$function = function(iterable $arg): iterable {};

$paramType = (new ReflectionParameter($function, 0))->getType();
var_dump($paramType::class);
var_dump($paramType);
var_dump($paramType->getName());
var_dump((string) $paramType);
var_dump($paramType->isBuiltin());

$reflectionFunc = new ReflectionFunction($function);
$returnType = $reflectionFunc->getReturnType();
var_dump($returnType::class);
var_dump($returnType);
var_dump($returnType->getName());
var_dump((string) $returnType);
var_dump($returnType->isBuiltin());

class PropIterableTypeTest {
    public iterable $iterable;
    public ?iterable $nullableIterable;
    public array $control;
    public ?array $nullableControl;
}

$reflector = new ReflectionClass(PropIterableTypeTest::class);

[$property, $nullable, $control, $nullableControl] = $reflector->getProperties();
$iterableType = $property->getType();
var_dump($iterableType::class);
var_dump($iterableType);
var_dump($iterableType->getName());
var_dump((string) $iterableType);
var_dump($iterableType->isBuiltin());

$nullableIterableType = $nullable->getType();
var_dump($nullableIterableType::class);
var_dump($nullableIterableType);
var_dump($nullableIterableType->getName());
var_dump((string) $nullableIterableType);
var_dump($nullableIterableType->isBuiltin());

$controlType = $control->getType();
var_dump($controlType::class);
var_dump($controlType);
var_dump($controlType->getName());
var_dump((string) $controlType);
var_dump($controlType->isBuiltin());

$nullableControlType = $nullableControl->getType();
var_dump($nullableControlType::class);
var_dump($nullableControlType);
var_dump($nullableControlType->getName());
var_dump((string) $nullableControlType);
var_dump($nullableControlType->isBuiltin());

?>
--EXPECTF--
string(19) "ReflectionNamedType"
object(ReflectionNamedType)#%d (0) {
}
string(8) "iterable"
string(8) "iterable"
bool(true)
string(19) "ReflectionNamedType"
object(ReflectionNamedType)#%d (0) {
}
string(8) "iterable"
string(8) "iterable"
bool(true)
string(19) "ReflectionNamedType"
object(ReflectionNamedType)#%d (0) {
}
string(8) "iterable"
string(8) "iterable"
bool(true)
string(19) "ReflectionNamedType"
object(ReflectionNamedType)#%d (0) {
}
string(8) "iterable"
string(9) "?iterable"
bool(true)
string(19) "ReflectionNamedType"
object(ReflectionNamedType)#%d (0) {
}
string(5) "array"
string(5) "array"
bool(true)
string(19) "ReflectionNamedType"
object(ReflectionNamedType)#%d (0) {
}
string(5) "array"
string(6) "?array"
bool(true)
