--TEST--
Test serialization
--EXTENSIONS--
uri
--XFAIL--
Unserialization support is missing
--FILE--
<?php

$uri1 = new Uri\Rfc3986\Uri("https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");
$serializedUri1 = serialize($uri1);
$uri2 = unserialize($serializedUri1);

var_dump($serializedUri1);
var_dump($uri2);

$uri1 = new Uri\WhatWg\Url("https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");
$serializedUri1 = serialize($uri1);
$uri2 = unserialize($serializedUri1);

var_dump($serializedUri1);
var_dump($uri2);

?>
--EXPECTF--
string(270) "O:14:"Uri\Rfc3986\Uri":8:{s:6:"scheme";s:5:"https";s:4:"user";s:8:"username";s:8:"password";s:8:"password";s:4:"host";s:14:"www.google.com";s:4:"port";i:8080;s:4:"path";s:29:"pathname1/pathname2/pathname3";s:5:"query";s:10:"query=true";s:8:"fragment";s:11:"hash-exists";}"
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  NULL
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(269) "O:13:"Uri\WhatWg\Url":8:{s:6:"scheme";s:5:"https";s:4:"user";s:8:"username";s:8:"password";s:8:"password";s:4:"host";s:14:"www.google.com";s:4:"port";i:8080;s:4:"path";s:29:"pathname1/pathname2/pathname3";s:5:"query";s:10:"query=true";s:8:"fragment";s:11:"hash-exists";}"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  NULL
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}

