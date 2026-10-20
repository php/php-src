--TEST--
Test Uri\WhatWg\UrlBuilder::setPassword() - success - empty string with credentials in base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('https://user:pass@example.com/base/path?oldQuery#oldFragment');

$url = new Uri\WhatWg\UrlBuilder()
    ->setPassword('')
    ->build($base);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals(new Uri\WhatWg\Url('', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(48) "https://user:pass@example.com/base/path?oldQuery"
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
  NULL
  ["path"]=>
  string(10) "/base/path"
  ["query"]=>
  string(8) "oldQuery"
  ["fragment"]=>
  NULL
}
bool(true)
bool(true)
