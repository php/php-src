--TEST--
Test Uri\QueryParams::append() - error - boolean value with strict types
--FILE--
<?php

declare(strict_types=1);

$params = Uri\QueryParams::parseRfc3986("x=1");

try {
    $result = $params->append("x", true);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: Uri\QueryParams::append(): Argument #2 ($value) must be of type BackedEnum|string|int|float|null, true given
