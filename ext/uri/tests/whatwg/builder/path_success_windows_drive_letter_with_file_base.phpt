--TEST--
Test Uri\WhatWg\UrlBuilder::setPath() - success - Windows drive letter with file base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('file:///C:/base/path?oldQuery#oldFragment');

$url = new Uri\WhatWg\UrlBuilder()
    ->setPath('D:/newPath')
    ->build($base);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals(new Uri\WhatWg\Url('./D:/newPath', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(26) "file:///C:/base/D:/newPath"
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
  string(19) "/C:/base/D:/newPath"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
bool(true)
