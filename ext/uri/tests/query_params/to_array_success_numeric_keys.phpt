--TEST--
Test Uri\QueryParams::toArray() - success - normal PHP key conversion at root and nested levels
--FILE--
<?php

$params = new Uri\QueryParams();
$params = $params->append("1", "a")->append("1", "b")->append("01", "c")->append("+1", "d")->append("-0", "e")->append("k[1]", "a")->append("k[01]", "b")->append("k[+1]", "c")->append("k[-0]", "d")->append("k[-1]", "e")->append("k[ 1]", "f");

var_dump($params->toArray());

?>
--EXPECT--
array(5) {
  [1]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "b"
  }
  ["01"]=>
  string(1) "c"
  ["+1"]=>
  string(1) "d"
  ["-0"]=>
  string(1) "e"
  ["k"]=>
  array(6) {
    [1]=>
    string(1) "a"
    ["01"]=>
    string(1) "b"
    ["+1"]=>
    string(1) "c"
    ["-0"]=>
    string(1) "d"
    [-1]=>
    string(1) "e"
    [" 1"]=>
    string(1) "f"
  }
}
