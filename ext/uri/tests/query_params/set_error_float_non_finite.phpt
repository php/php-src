--TEST--
Test Uri\QueryParams::set() - error - non-finite float leaves the original unchanged
--FILE--
<?php

$params1 = (new Uri\QueryParams())->append("x", "1");

try {
    $params2 = $params1->set("x", INF);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump($params1->toRfc3986String());

?>
--EXPECT--
ValueError: Uri\QueryParams::set(): Argument #2 ($value) must be finite
string(3) "x=1"
