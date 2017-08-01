--TEST--
Test http_query_decode() function : test with badly formed strings
--FILE--
<?php

/* Prototype:   array http_query_decode(string $query)
 * Description: Parses the string into variables
 * Source code: ext/standard/http.c
*/

echo "\nTest string with badly formed strings\n";
$query = "arr[1=sid&arr[4][2=fred";
var_dump(http_query_decode($query));

$query = "arr1]=sid&arr[4]2]=fred";
var_dump(http_query_decode($query));

$query = "arr[one=sid&arr[4][two=fred";
var_dump(http_query_decode($query));

echo "\nTest string with badly formed % numbers\n";
$query = "first=%41&second=%a&third=%b";
var_dump(http_query_decode($query));

echo "\nTest string with non-binary safe name\n";
$query = "arr.test[1]=sid&arr test[4][two]=fred";
var_dump(http_query_decode($query));

?>
===DONE===
--EXPECTF--
Test string with badly formed strings
array(2) {
  ["arr_1"]=>
  string(3) "sid"
  ["arr"]=>
  array(1) {
    [4]=>
    string(4) "fred"
  }
}
array(2) {
  ["arr1]"]=>
  string(3) "sid"
  ["arr"]=>
  array(1) {
    [4]=>
    string(4) "fred"
  }
}
array(2) {
  ["arr_one"]=>
  string(3) "sid"
  ["arr"]=>
  array(1) {
    [4]=>
    string(4) "fred"
  }
}

Test string with badly formed % numbers
array(3) {
  ["first"]=>
  string(1) "A"
  ["second"]=>
  string(2) "%a"
  ["third"]=>
  string(2) "%b"
}

Test string with non-binary safe name
array(1) {
  ["arr_test"]=>
  array(2) {
    [1]=>
    string(3) "sid"
    [4]=>
    array(1) {
      ["two"]=>
      string(4) "fred"
    }
  }
}
===DONE===
