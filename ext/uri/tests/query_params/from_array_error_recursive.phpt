--TEST--
Test Uri\QueryParams::fromArray() - error - recursive input and recovery
--FILE--
<?php

$array = ["value" => 1];
$array["self"] = &$array;

try {
    Uri\QueryParams::fromArray($array);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

unset($array["self"]);
var_dump(Uri\QueryParams::fromArray($array)->toList());

?>
--EXPECT--
ValueError: Uri\QueryParams::fromArray(): Argument #1 ($queryParams) must not contain recursive arrays
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(5) "value"
    [1]=>
    string(1) "1"
  }
}
