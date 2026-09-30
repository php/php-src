--TEST--
Test Uri\QueryParams::append() - error - object
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("");

try {
    $params = $params->append("foo", new stdClass());
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: Uri\QueryParams::append(): Argument #2 ($value) must be of type BackedEnum|string|int|float|null, stdClass given
