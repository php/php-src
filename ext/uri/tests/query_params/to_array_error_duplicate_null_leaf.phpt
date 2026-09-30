--TEST--
Test Uri\QueryParams::toArray() - error - null is an occupied leaf
--FILE--
<?php

$params = new Uri\QueryParams();
$params = $params->append("k[a]", null)->append("k[a]", "2");

try {
    $params->toArray();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidQueryParamsException: The query parameter "k[a]" has conflicting array paths
