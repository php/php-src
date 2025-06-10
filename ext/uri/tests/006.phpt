--TEST--
Test successful manual Uri child instance creation
--EXTENSIONS--
uri
--FILE--
<?php

$url = new Uri\WhatWg\Url("https://username:password@example.com:8080/path?q=r#fragment");
var_dump($url);

?>
--EXPECTF--
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  string(8) "username"
  ["password"]=>
  string(8) "password"
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  int(8080)
  ["path"]=>
  string(5) "/path"
  ["query"]=>
  string(3) "q=r"
  ["fragment"]=>
  string(8) "fragment"
}
