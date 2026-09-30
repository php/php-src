--TEST--
Test Uri\QueryParams::append() - error - resource
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("");

try {
    $params = $params->append("foo", fopen("php://memory", "r"));
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: Uri\QueryParams::append(): Argument #2 ($value) must be of type BackedEnum|string|int|float|null, resource given
