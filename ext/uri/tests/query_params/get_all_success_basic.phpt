--TEST--
Test Uri\QueryParams::getAll() - success - strings, null and empty values in encounter order
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("x=first&other=ignored&x&x=&x=last");
$values = $params->getAll("x");
unset($params);

var_dump($values);

?>
--EXPECT--
array(4) {
  [0]=>
  string(5) "first"
  [1]=>
  NULL
  [2]=>
  string(0) ""
  [3]=>
  string(4) "last"
}
