--TEST--
Test Uri\WhatWg\UrlBuilder::setPath() - success - relative path with non-special base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('scheme://user:pass@example.com:123/base/path?oldQuery#oldFragment');

$url = new Uri\WhatWg\UrlBuilder()
    ->setPath('newPath')
    ->build($base);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals(new Uri\WhatWg\Url('newPath', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(47) "scheme://user:pass@example.com:123/base/newPath"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(6) "scheme"
  ["username"]=>
  string(4) "user"
  ["password"]=>
  string(4) "pass"
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  int(123)
  ["path"]=>
  string(13) "/base/newPath"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
bool(true)
