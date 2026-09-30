--TEST--
Test Uri\Rfc3986\UriBuilder::setPath() - success - dot-segments with base URI
--FILE--
<?php

$base = new Uri\Rfc3986\Uri('https://user:pass@example.com:123/base/path?oldQuery#oldFragment');

$uri = new Uri\Rfc3986\UriBuilder()
    ->setPath('../newPath/./child')
    ->build($base);

var_dump($uri->toRawString());
var_dump($uri);
var_dump($uri->equals(new Uri\Rfc3986\Uri($uri->toRawString())));
var_dump($uri->equals(new Uri\Rfc3986\Uri('../newPath/./child', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(47) "https://user:pass@example.com:123/newPath/child"
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
  string(14) "/newPath/child"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
bool(true)
