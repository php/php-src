--TEST--
Test Uri\WhatWg\UrlBuilder::setHost() - success - with file base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('file:///Users/example/base/path?oldQuery#oldFragment');

$url = new Uri\WhatWg\UrlBuilder()
    ->setHost('example.net')
    ->build($base);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals(new Uri\WhatWg\Url('//example.net', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(19) "file://example.net/"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(4) "file"
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
