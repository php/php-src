--TEST--
Test Uri\WhatWg\UrlBuilder basic - success - absolute URL with base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('https://user:pass@example.com:123/base/path?oldQuery#oldFragment');

$url = new Uri\WhatWg\UrlBuilder()
    ->setScheme('http')
    ->setHost('example.net')
    ->setPath('/newPath')
    ->build($base);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals(new Uri\WhatWg\Url('http://example.net/newPath', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(26) "http://example.net/newPath"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(4) "http"
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
