--TEST--
Test Uri\WhatWg\UrlBuilder::setPath() - success - question mark with base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('https://user:pass@example.com:123/base/path?oldQuery#oldFragment');

$url = new Uri\WhatWg\UrlBuilder()
    ->setPath('newPath?value')
    ->build($base);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals(new Uri\WhatWg\Url('newPath%3Fvalue', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(54) "https://user:pass@example.com:123/base/newPath%3Fvalue"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  string(4) "user"
  ["password"]=>
  string(4) "pass"
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  int(123)
  ["path"]=>
  string(21) "/base/newPath%3Fvalue"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
bool(true)
