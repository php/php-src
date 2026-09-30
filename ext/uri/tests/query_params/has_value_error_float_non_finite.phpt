--TEST--
Test Uri\QueryParams::hasValue() - error - non-finite float
--FILE--
<?php

$params = (new Uri\QueryParams())->append("x", "NAN");

try {
    $result = $params->hasValue("x", NAN);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump($params->hasValue("x", "NAN"));

?>
--EXPECT--
ValueError: Uri\QueryParams::hasValue(): Argument #2 ($value) must be finite
bool(true)
