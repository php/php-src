--TEST--
Test Uri\QueryParams::append() - error - negative infinity
--FILE--
<?php

$params1 = new Uri\QueryParams();

try {
    $params2 = $params1->append("x", -INF);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump($params1->count());

?>
--EXPECT--
ValueError: Uri\QueryParams::append(): Argument #2 ($value) must be finite
int(0)
