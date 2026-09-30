--TEST--
Test Uri\QueryParams::append() - error - arrays use the array API
--FILE--
<?php

$params = new Uri\QueryParams();

try {
    $params = $params->append("k", [1, 2]);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: Uri\QueryParams::append(): Argument #2 ($value) must be of type BackedEnum|string|int|float|null, array given
