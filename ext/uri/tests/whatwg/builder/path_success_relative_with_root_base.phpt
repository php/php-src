--TEST--
Test Uri\WhatWg\UrlBuilder::setPath() - success - relative path with root base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('https://example.com/');

$url = new Uri\WhatWg\UrlBuilder()
    ->setPath('newPath')
    ->build($base);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals(new Uri\WhatWg\Url('newPath', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(27) "https://example.com/newPath"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
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
