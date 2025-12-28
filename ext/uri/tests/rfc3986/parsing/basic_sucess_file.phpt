--TEST--
Test Uri\Rfc3986\Uri parsing - basic - file
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("file:///E:/Documents%20and%20Settings");

var_dump($uri);
var_dump($uri->toRawString());
var_dump($uri->toString());

?>
--EXPECTF--
object(Uri\Rfc3986\Uri)#%d (%d) {
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
string(37) "file:///E:/Documents%20and%20Settings"
