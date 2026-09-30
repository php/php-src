--TEST--
Test Uri\QueryParams::set() - error - boolean value
--FILE--
<?php

declare(strict_types=1);

$params = Uri\QueryParams::parseRfc3986("x=1");

try {
    $result = $params->set("x", false);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: Uri\QueryParams::set(): Argument #2 ($value) must be of type BackedEnum|string|int|float|null, false given
