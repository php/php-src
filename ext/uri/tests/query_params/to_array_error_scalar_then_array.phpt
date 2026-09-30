--TEST--
Test Uri\QueryParams::toArray() - error - scalar then array
--FILE--
<?php

$params = new Uri\QueryParams();
$params = $params->append("k[a]", "1")->append("k[a][b]", "2");

try {
    $params->toArray();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidQueryParamsException: The query parameter "k[a][b]" has conflicting array paths
