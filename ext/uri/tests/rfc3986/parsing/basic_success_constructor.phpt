--TEST--
Test Uri\Rfc3986\Uri parsing - basic - parsing all components with the constructor
--FILE--
<?php

$uri = new Uri\Rfc3986\Uri("https://username:password@example.com:8080/path?q=r#fragment");

var_dump($uri);
var_dump($uri->toRawString());
var_dump($uri->toString());

?>
--EXPECTF--
object(Uri\Rfc3986\Uri)#%d (%d) {
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
string(60) "https://username:password@example.com:8080/path?q=r#fragment"
string(60) "https://username:password@example.com:8080/path?q=r#fragment"
