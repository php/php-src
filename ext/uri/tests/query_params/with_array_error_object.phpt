--TEST--
Test Uri\QueryParams::withArray() - error - invalid nested value leaves the original unchanged
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("old=1&old=2");

try {
    $params = $params->withArray(["new" => 3, "bad" => [new stdClass()]]);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

var_dump($params->toList());
var_dump($params->getAll("old"));

?>
--EXPECT--
TypeError: Uri\QueryParams::withArray(): Argument #1 ($queryParams) contains an invalid value for query parameter "bad[]": expected BackedEnum|string|int|float|null, stdClass given
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(3) "old"
    [1]=>
    string(1) "1"
  }
  [1]=>
  array(2) {
    [0]=>
    string(3) "old"
    [1]=>
    string(1) "2"
  }
}
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "2"
}
