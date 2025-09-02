--TEST--
Test reference resolution
--EXTENSIONS--
uri
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("https://example.com/without-base");
var_dump($uri);

$uri = Uri\Rfc3986\Uri::parse("/with-base", new Uri\Rfc3986\Uri("https://example.com"));
var_dump($uri);

$uri = Uri\Rfc3986\Uri::parse("https://test.com/with-base-in-vain", new Uri\Rfc3986\Uri("https://example.com"));
var_dump($uri);

$uri = Uri\Rfc3986\Uri::parse("HTTPS://TEST.com/with-base-in-vain", Uri\Rfc3986\Uri::parse("https://example.com"));
var_dump($uri);
var_dump($uri->toString());

$url = Uri\WhatWg\Url::parse("https://example.com/without-base/");
var_dump($url);

$url = Uri\WhatWg\Url::parse("/with-base", new Uri\WhatWg\Url("https://example.com"));
var_dump($url);

$url = Uri\WhatWg\Url::parse("https://test.com/with-base-in-vain", Uri\WhatWg\Url::parse("https://example.com/"));
var_dump($url);
?>
--EXPECTF--
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(13) "/without-base"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(10) "/with-base"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(8) "test.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(18) "/with-base-in-vain"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "HTTPS"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(8) "TEST.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(18) "/with-base-in-vain"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(34) "https://test.com/with-base-in-vain"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(14) "/without-base/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(10) "/with-base"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(8) "test.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(18) "/with-base-in-vain"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
