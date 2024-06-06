--TEST--
Test manual Url/Url instance creation
--EXTENSIONS--
url
--FILE--
<?php

$url = new Url\Url("https", "example.com", null, null, null, null, null, null);
var_dump($url);

$url = new Url\Url("https", "example.com", 8080, "user", "password", "/path", "q=r", "fragment");
var_dump($url);

?>
--EXPECTF--
object(Url\Url)#%d (%d) {
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
object(Url\Url)#%d (%d) {
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
