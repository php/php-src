--TEST--
Test Uri\WhatWg\UrlBuilder::setQuery() - success - with scheme-relative URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('https://user:pass@example.com:123/base/path?oldQuery#oldFragment');

$url = new Uri\WhatWg\UrlBuilder()
    ->setHost('example.net')
    ->setPath('/newPath')
    ->setQuery('newQuery')
    ->build($base);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals(new Uri\WhatWg\Url('//example.net/newPath?newQuery', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(36) "https://example.net/newPath?newQuery"
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
  string(8) "newQuery"
  ["fragment"]=>
  NULL
}
bool(true)
bool(true)
