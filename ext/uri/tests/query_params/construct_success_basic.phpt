--TEST--
Test Uri\QueryParams::__construct() - success - basic
--FILE--
<?php

$params = new Uri\QueryParams();

var_dump($params);
var_dump($params->toRfc3986String());

?>
--EXPECTF--
object(Uri\QueryParams)#%d (%d) {
  ["options":"Uri\QueryParams":private]=>
  object(Uri\QueryParamOptions)#%d (%d) {
    ["parsingMaxQueryStringLength"]=>
    int(10000)
    ["parsingMaxParamCount"]=>
    int(1000)
    ["trueValue"]=>
    string(1) "1"
    ["falseValue"]=>
    string(1) "0"
    ["useNullAsLong"]=>
    bool(false)
  }
  ["params"]=>
  array(0) {
  }
}
string(0) ""
