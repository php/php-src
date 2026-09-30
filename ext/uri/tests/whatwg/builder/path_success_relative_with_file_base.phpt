--TEST--
Test Uri\WhatWg\UrlBuilder::setPath() - success - relative path with file base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('file:///base/path');

$url = new Uri\WhatWg\UrlBuilder()
    ->setPath('newPath')
    ->build($base);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals(new Uri\WhatWg\Url('newPath', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(20) "file:///base/newPath"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(4) "file"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(0) ""
  ["port"]=>
  NULL
  ["path"]=>
  string(13) "/base/newPath"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
bool(true)
