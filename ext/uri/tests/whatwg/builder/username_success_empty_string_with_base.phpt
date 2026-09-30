--TEST--
Test Uri\WhatWg\UrlBuilder::setUsername() - success - empty string with base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('https://oldUser:oldPass@example.com:123/base/path?oldQuery#oldFragment');

$url = new Uri\WhatWg\UrlBuilder()
    ->setUsername('')
    ->setHost('example.net')
    ->build($base);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals(new Uri\WhatWg\Url('//example.net', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(20) "https://example.net/"
object(Uri\WhatWg\Url)#%d (8) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.net"
  ["port"]=>
  NULL
  ["path"]=>
  string(1) "/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
bool(true)
