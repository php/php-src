--TEST--
Test Uri\QueryParams::toArray() - error - depth beyond the default limit
--FILE--
<?php

$name = "k" . str_repeat("[a]", 65);
$params = (new Uri\QueryParams())->append($name, "value");
try {
    $params->toArray();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidQueryParamsException: The query parameter "k[a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a][a]" exceeds the maximum nesting level of 64
