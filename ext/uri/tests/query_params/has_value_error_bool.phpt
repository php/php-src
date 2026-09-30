--TEST--
Test Uri\QueryParams::hasValue() - error - boolean value with strict types
--FILE--
<?php

declare(strict_types=1);

$params = Uri\QueryParams::parseRfc3986("x=1");

try {
    $result = $params->hasValue("x", true);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: Uri\QueryParams::hasValue(): Argument #2 ($value) must be of type BackedEnum|string|int|float|null, true given
