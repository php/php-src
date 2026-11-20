--TEST--
Test Uri\QueryParams::toArray() - error - appending beyond the largest integer key
--FILE--
<?php

$params = new Uri\QueryParams();
$params = $params->append("k[" . PHP_INT_MAX . "]", "1")->append("k[]", "2");

try {
    $params->toArray();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidQueryParamsException: Cannot append an array element for query parameter "k[]"
