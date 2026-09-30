--TEST--
Test Uri\WhatWg\UrlBuilder::setScheme() - success - matching non-special scheme with base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('scheme://user:pass@example.com:123/base/path?oldQuery#oldFragment');

$url = new Uri\WhatWg\UrlBuilder()
    ->setScheme('scheme')
    ->setPath('opaquePath')
    ->build($base);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals(new Uri\WhatWg\Url('scheme:opaquePath', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(17) "scheme:opaquePath"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(6) "scheme"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(10) "opaquePath"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
bool(true)
