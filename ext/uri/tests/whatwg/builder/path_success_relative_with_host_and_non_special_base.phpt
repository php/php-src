--TEST--
Test Uri\WhatWg\UrlBuilder::setPath() - success - relative path with host and non-special base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('foo://example.com/base/path');

$url = new Uri\WhatWg\UrlBuilder()
    ->setHost('example.net')
    ->setPath('newPath')
    ->build($base);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals(new Uri\WhatWg\Url('//example.net/newPath', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(25) "foo://example.net/newPath"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(3) "foo"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.net"
  ["port"]=>
  NULL
  ["path"]=>
  string(8) "/newPath"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
bool(true)
