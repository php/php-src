--TEST--
Test Uri\Rfc3986\UriBuilder::setPath() - success - relative path with base URI
--FILE--
<?php

$base = new Uri\Rfc3986\Uri('https://user:pass@example.com:123/base/path?oldQuery#oldFragment');

$uri = new Uri\Rfc3986\UriBuilder()
    ->setPath('newPath')
    ->build($base);

var_dump($uri->toRawString());
var_dump($uri);
var_dump($uri->equals(new Uri\Rfc3986\Uri($uri->toRawString())));
var_dump($uri->equals(new Uri\Rfc3986\Uri('newPath', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(46) "https://user:pass@example.com:123/base/newPath"
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
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
