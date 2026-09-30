--TEST--
Test Uri\QueryParams::toArray() - error - duplicate leaf
--FILE--
<?php

$params = new Uri\QueryParams();
$params = $params->append("k[a]", "1")->append("k[a]", "2");

try {
    $params->toArray();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidQueryParamsException: The query parameter "k[a]" has conflicting array paths
