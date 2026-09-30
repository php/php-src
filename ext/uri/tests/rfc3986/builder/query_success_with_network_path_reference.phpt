--TEST--
Test Uri\Rfc3986\UriBuilder::setQuery() - success - with network-path reference
--FILE--
<?php

$base = new Uri\Rfc3986\Uri('https://user:pass@example.com:123/base/path?oldQuery#oldFragment');

$uri = new Uri\Rfc3986\UriBuilder()
    ->setHost('example.net')
    ->setPath('/newPath')
    ->setQuery('newQuery')
    ->build($base);

var_dump($uri->toRawString());
var_dump($uri);
var_dump($uri->equals(new Uri\Rfc3986\Uri($uri->toRawString())));
var_dump($uri->equals(new Uri\Rfc3986\Uri('//example.net/newPath?newQuery', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(36) "https://example.net/newPath?newQuery"
object(Uri\Rfc3986\Uri)#%d (%d) {
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
