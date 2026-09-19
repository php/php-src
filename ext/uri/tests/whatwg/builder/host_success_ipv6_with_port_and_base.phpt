--TEST--
Test Uri\WhatWg\UrlBuilder::setHost() - success - IPv6 address with port and base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('https://example.com/base/path');

$url = new Uri\WhatWg\UrlBuilder()
    ->setHost('[2001:db8::1]')
    ->setPort(124)
    ->build($base);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals(new Uri\WhatWg\Url('//[2001:db8::1]:124', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(26) "https://[2001:db8::1]:124/"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(13) "[2001:db8::1]"
  ["port"]=>
  int(124)
  ["path"]=>
  string(1) "/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
bool(true)
