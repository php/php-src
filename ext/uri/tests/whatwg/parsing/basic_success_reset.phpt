--TEST--
Test Uri\WhatWg\Url::parse() - success - clears errors when there are no validation errors
--FILE--
<?php

$errors = ["previous error"];
$url = Uri\WhatWg\Url::parse("https://example.org/", errors: $errors);

var_dump($url);
var_dump($url->toAsciiString());
var_dump($errors);

?>
--EXPECTF--
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.org"
  ["port"]=>
  NULL
  ["path"]=>
  string(1) "/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(20) "https://example.org/"
array(0) {
}
