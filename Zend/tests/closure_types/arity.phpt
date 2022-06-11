--TEST--
Closure type arity
--FILE--
<?php

function test(string $closureType, string $params) {
    try {
        echo ($closureType ?: "''") . ' > ' . ($params ?: "''") . ': ';
        eval("(function (fn ($closureType) \$c) {})(function($params) {});");
        echo 'true';
    } catch (\Throwable $e) {
        echo $e->getMessage();
    }
    echo "\n";
}

$closureTypes = [
    '',
    'mixed',
    'mixed&',
    'mixed, mixed',
    'mixed...',
    'mixed, mixed...',
];

$params = [
    '',
    '$a',
    '$a = null',
    '&$a',
    '$a, $b',
    '$a, $b',
];

foreach ($closureTypes as $closureType) {
    foreach ($params as $param) {
        test($closureType, $param);
    }
}

?>
--EXPECTF--
'' > '': true
'' > $a: {closure}(): Argument #1 ($c) must be of type fn(), Closure given, called in %s on line %d
'' > $a = null: true
'' > &$a: {closure}(): Argument #1 ($c) must be of type fn(), Closure given, called in %s on line %d
'' > $a, $b: {closure}(): Argument #1 ($c) must be of type fn(), Closure given, called in %s on line %d
'' > $a, $b: {closure}(): Argument #1 ($c) must be of type fn(), Closure given, called in %s on line %d
mixed > '': true
mixed > $a: true
mixed > $a = null: true
mixed > &$a: true
mixed > $a, $b: {closure}(): Argument #1 ($c) must be of type fn(mixed), Closure given, called in %s on line %d
mixed > $a, $b: {closure}(): Argument #1 ($c) must be of type fn(mixed), Closure given, called in %s on line %d
mixed& > '': true
mixed& > $a: true
mixed& > $a = null: true
mixed& > &$a: true
mixed& > $a, $b: {closure}(): Argument #1 ($c) must be of type fn(mixed&), Closure given, called in %s on line %d
mixed& > $a, $b: {closure}(): Argument #1 ($c) must be of type fn(mixed&), Closure given, called in %s on line %d
mixed, mixed > '': true
mixed, mixed > $a: true
mixed, mixed > $a = null: true
mixed, mixed > &$a: true
mixed, mixed > $a, $b: true
mixed, mixed > $a, $b: true
mixed... > '': true
mixed... > $a: true
mixed... > $a = null: true
mixed... > &$a: true
mixed... > $a, $b: true
mixed... > $a, $b: true
mixed, mixed... > '': true
mixed, mixed... > $a: true
mixed, mixed... > $a = null: true
mixed, mixed... > &$a: true
mixed, mixed... > $a, $b: true
mixed, mixed... > $a, $b: true
