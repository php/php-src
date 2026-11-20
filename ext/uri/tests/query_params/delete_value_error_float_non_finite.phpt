--TEST--
Test Uri\QueryParams::deleteValue() - error - non-finite float leaves the original unchanged
--FILE--
<?php

$params1 = (new Uri\QueryParams())->append("x", "-INF");

try {
    $params2 = $params1->deleteValue("x", -INF);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump($params1->toRfc3986String());

?>
--EXPECT--
ValueError: Uri\QueryParams::deleteValue(): Argument #2 ($value) must be finite
string(6) "x=-INF"
