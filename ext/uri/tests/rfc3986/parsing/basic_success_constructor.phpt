--TEST--
Test Uri\Rfc3986\Uri construction - basic - success
--FILE--
<?php

var_dump(new Uri\Rfc3986\Uri("https://username:password@example.com:8080/path?q=r#fragment"));

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
