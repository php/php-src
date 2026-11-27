--TEST--
Test Uri\QueryParams::deleteValue() - error - resource
--FILE--
<?php

$value = fopen("php://memory", "r+");

$params = Uri\QueryParams::parseRfc3986("x=1");

try {
    $params = $params->deleteValue("x", $value);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump($params->toRfc3986String());

?>
--EXPECT--
TypeError: Uri\QueryParams::deleteValue(): Argument #2 ($value) must be of type BackedEnum|string|int|float|null, resource given
string(3) "x=1"
