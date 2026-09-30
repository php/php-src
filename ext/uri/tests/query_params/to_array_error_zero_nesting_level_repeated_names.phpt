--TEST--
Test Uri\QueryParams::toArray() - error - zero nesting level repeated names
--FILE--
<?php

$params = new Uri\QueryParams();
$params = $params->append("k", "1")->append("k", "2");

try {
    $params->toArray(0);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidQueryParamsException: The query parameter "k" exceeds the maximum nesting level of 0
