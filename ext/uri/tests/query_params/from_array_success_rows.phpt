--TEST--
Test Uri\QueryParams::fromArray() - success - maps within a list retain their grouping
--FILE--
<?php

$params = Uri\QueryParams::fromArray(["rows" => [["a" => 1, "b" => 2], ["a" => 3, "b" => 4]]]);

var_dump($params->toList());
var_dump($params->toArray());

?>
--EXPECT--
array(4) {
  [0]=>
  array(2) {
    [0]=>
    string(10) "rows[0][a]"
    [1]=>
    string(1) "1"
  }
  [1]=>
  array(2) {
    [0]=>
    string(10) "rows[0][b]"
    [1]=>
    string(1) "2"
  }
  [2]=>
  array(2) {
    [0]=>
    string(10) "rows[1][a]"
    [1]=>
    string(1) "3"
  }
  [3]=>
  array(2) {
    [0]=>
    string(10) "rows[1][b]"
    [1]=>
    string(1) "4"
  }
}
array(1) {
  ["rows"]=>
  array(2) {
    [0]=>
    array(2) {
      ["a"]=>
      string(1) "1"
      ["b"]=>
      string(1) "2"
    }
    [1]=>
    array(2) {
      ["a"]=>
      string(1) "3"
      ["b"]=>
      string(1) "4"
    }
  }
}
