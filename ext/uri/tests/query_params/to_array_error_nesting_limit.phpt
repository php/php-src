--TEST--
Test Uri\QueryParams::toArray() - error - nesting limit
--FILE--
<?php

$params = new Uri\QueryParams();
$params = $params->append("k[a][b]", "1");

try {
    $params->toArray(1);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidQueryParamsException: The query parameter "k[a][b]" exceeds the maximum nesting level of 1
