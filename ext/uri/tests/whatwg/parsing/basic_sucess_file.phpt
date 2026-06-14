--TEST--
Test Uri\WhatWg\Url parsing - basic - file
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("file:///E:/Documents%20and%20Settings");

var_dump($url);
var_dump($url->toAsciiString());

?>
--EXPECTF--
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(4) "file"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(0) ""
  ["port"]=>
  NULL
  ["path"]=>
  string(30) "/E:/Documents%20and%20Settings"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(37) "file:///E:/Documents%20and%20Settings"
