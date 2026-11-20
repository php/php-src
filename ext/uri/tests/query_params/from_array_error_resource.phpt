--TEST--
Test Uri\QueryParams::fromArray() - error - nested resource
--FILE--
<?php

$resource = fopen("php://memory", "r+");
try {
    Uri\QueryParams::fromArray(["nested" => [$resource]]);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
fclose($resource);

?>
--EXPECT--
TypeError: Uri\QueryParams::fromArray(): Argument #1 ($queryParams) contains an invalid value for query parameter "nested[]": expected BackedEnum|string|int|float|null, resource given
