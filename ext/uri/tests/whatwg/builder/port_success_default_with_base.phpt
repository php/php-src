--TEST--
Test Uri\WhatWg\UrlBuilder::setPort() - success - default port with base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('https://example.com/base/path');

$url = new Uri\WhatWg\UrlBuilder()
    ->setHost('example.net')
    ->setPort(443)
    ->build($base);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals(new Uri\WhatWg\Url('//example.net:443', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(20) "https://example.net/"
object(Uri\WhatWg\Url)#%d (%d) {
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
