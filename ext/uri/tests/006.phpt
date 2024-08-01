--TEST--
Test manual Uri instance creation
--EXTENSIONS--
uri
--FILE--
<?php

$url = new Uri\Uri("https", "example.com");
var_dump($url);

$url = new Uri\Uri("https", "example.com", 8080, "user", "password", "/path", "q=r", "fragment");
var_dump($url);

?>
--EXPECTF--
object(Uri\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["path"]=>
  NULL
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(Uri\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  int(8080)
  ["user"]=>
  string(4) "user"
  ["password"]=>
  string(8) "password"
  ["path"]=>
  string(5) "/path"
  ["query"]=>
  string(3) "q=r"
  ["fragment"]=>
  string(8) "fragment"
}
