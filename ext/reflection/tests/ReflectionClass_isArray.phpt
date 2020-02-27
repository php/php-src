--TEST--
public bool ReflectionParameter::isArray ( void );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - @phpsp - sao paulo - br
--FILE--
<?php

function testReflectionIsArray(array $a, ?array $b, iterable $c, array|string $d) {}

$reflection = new ReflectionFunction('testReflectionIsArray');

foreach ($reflection->getParameters() as $parameter) {
    var_dump($parameter->isArray());
}
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
