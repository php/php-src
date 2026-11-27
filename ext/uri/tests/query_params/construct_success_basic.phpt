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
  ["params"]=>
  array(0) {
  }
}
string(0) ""
