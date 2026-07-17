--TEST--
Test Uri\QueryParams::append() - error - object
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("");

try {
    $params->append("foo", new stdClass());
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: Uri\QueryParams::append(): Argument #2 ($value) must not contain an object
