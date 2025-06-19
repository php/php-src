--TEST--
Test reference resolution
--EXTENSIONS--
uri
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("https://example.com/without-base/");
var_dump($url);

$url = Uri\WhatWg\Url::parse("/with-base", new Uri\WhatWg\Url("https://example.com"));
var_dump($url);

$url = Uri\WhatWg\Url::parse("https://test.com/with-base-in-vain", Uri\WhatWg\Url::parse("https://example.com/"));
var_dump($url);
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
