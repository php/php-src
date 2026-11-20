--TEST--
Test Uri\QueryParams::withArray() - error - nested non-finite float leaves the original unchanged
--FILE--
<?php

$params1 = (new Uri\QueryParams())->append("old", "1");

try {
    $params2 = $params1->withArray(["new" => 2, "nested" => [-INF]]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump($params1->toRfc3986String());

?>
--EXPECT--
ValueError: Uri\QueryParams::withArray(): Argument #1 ($queryParams) contains a non-finite float for query parameter "nested[]"
string(5) "old=1"
