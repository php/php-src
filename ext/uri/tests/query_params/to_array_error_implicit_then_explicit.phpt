--TEST--
Test Uri\QueryParams::toArray() - error - implicit then explicit
--FILE--
<?php

$params = new Uri\QueryParams();
$params = $params->append("k[]", "1")->append("k[0]", "2");

try {
    $params->toArray();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidQueryParamsException: The query parameter "k[0]" has conflicting array paths
