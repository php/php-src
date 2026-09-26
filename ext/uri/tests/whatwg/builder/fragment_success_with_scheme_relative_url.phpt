--TEST--
Test Uri\WhatWg\UrlBuilder::setFragment() - success - with scheme-relative URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('https://user:pass@example.com:123/base/path?oldQuery#oldFragment');

$url = new Uri\WhatWg\UrlBuilder()
    ->setHost('example.net')
    ->setPath('/newPath')
    ->setFragment('newFragment')
    ->build($base);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals(new Uri\WhatWg\Url('//example.net/newPath#newFragment', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(39) "https://example.net/newPath#newFragment"
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
  string(8) "/newPath"
  ["query"]=>
  NULL
  ["fragment"]=>
  string(11) "newFragment"
}
bool(true)
bool(true)
