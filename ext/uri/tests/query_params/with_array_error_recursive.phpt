--TEST--
Test Uri\QueryParams::withArray() - error - recursive array leaves the original unchanged
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("old=1");
$array = [];
$array["self"] = &$array;

try {
    $params = $params->withArray($array);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

var_dump($params->toList());

?>
--EXPECT--
ValueError: Uri\QueryParams::withArray(): Argument #1 ($queryParams) must not contain recursive arrays
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(3) "old"
    [1]=>
    string(1) "1"
  }
}
