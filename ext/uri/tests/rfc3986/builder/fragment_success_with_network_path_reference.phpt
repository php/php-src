--TEST--
Test Uri\Rfc3986\UriBuilder::setFragment() - success - with network-path reference
--FILE--
<?php

$base = new Uri\Rfc3986\Uri('https://user:pass@example.com:123/base/path?oldQuery#oldFragment');

$uri = new Uri\Rfc3986\UriBuilder()
    ->setHost('example.net')
    ->setPath('/newPath')
    ->setFragment('newFragment')
    ->build($base);

var_dump($uri->toRawString());
var_dump($uri);
var_dump($uri->equals(new Uri\Rfc3986\Uri($uri->toRawString())));
var_dump($uri->equals(new Uri\Rfc3986\Uri('//example.net/newPath#newFragment', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(39) "https://example.net/newPath#newFragment"
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
  NULL
  ["fragment"]=>
  string(11) "newFragment"
}
bool(true)
bool(true)
