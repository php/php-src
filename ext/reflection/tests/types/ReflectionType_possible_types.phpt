--TEST--
ReflectionType possible types
--FILE--
<?php

$functions = [
    function(): void {},
    function(): int {},
    function(): float {},
    function(): string {},
    function(): bool {},
    function(): array {},
    function(): callable {},
    function(): null {},
    function(): false {},
    function(): true {},
    function(): StdClass {}
];

foreach ($functions as $function) {
    $reflectionFunc = new ReflectionFunction($function);
    $returnType = $reflectionFunc->getReturnType();
    var_dump($returnType->getName());
}
?>
--EXPECT--
string(4) "void"
string(3) "int"
string(5) "float"
string(6) "string"
string(4) "bool"
string(5) "array"
string(8) "callable"
string(4) "null"
string(5) "false"
string(4) "true"
string(8) "StdClass"
