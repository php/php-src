--TEST--
Test Uri\QueryParams::fromArray() - error - indirect recursion
--FILE--
<?php

$left = [];
$right = ["left" => &$left];
$left["right"] = &$right;

try {
    Uri\QueryParams::fromArray($left);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: Uri\QueryParams::fromArray(): Argument #1 ($queryParams) must not contain recursive arrays
