--TEST--
Test Uri\QueryParams::__construct() - success - with options set
--FILE--
<?php

$params = new Uri\QueryParams(new Uri\QueryParamOptions(100, 10, "true", "false", true));

var_dump($params);
var_dump($params->toRfc3986String());

?>
--EXPECTF--
object(Uri\QueryParams)#%d (%d) {
  ["options":"Uri\QueryParams":private]=>
  object(Uri\QueryParamOptions)#%d (%d) {
    ["parsingMaxQueryStringLength"]=>
    int(100)
    ["parsingMaxParamCount"]=>
    int(10)
    ["trueValue"]=>
    string(4) "true"
    ["falseValue"]=>
    string(5) "false"
    ["useNullAsLong"]=>
    bool(true)
  }
  ["params"]=>
  array(0) {
  }
}
string(0) ""
