--TEST--
Test Uri\QueryParams::append() - error - resource
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("");

try {
    $params->append("foo", fopen("php://memory", "r"));
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: Uri\QueryParams::append(): Argument #2 ($value) must not contain a resource
