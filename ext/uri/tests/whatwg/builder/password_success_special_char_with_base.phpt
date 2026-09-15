--TEST--
Test Uri\WhatWg\UrlBuilder::setPassword() - success - special characters with base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('https://example.com/base/path');

$url = new Uri\WhatWg\UrlBuilder()
    ->setPassword('p:@')
    ->setHost('example.net')
    ->build($base);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals(new Uri\WhatWg\Url('//:p%3A%40@example.net', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(29) "https://:p%3A%40@example.net/"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  string(0) ""
  ["password"]=>
  string(7) "p%3A%40"
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
