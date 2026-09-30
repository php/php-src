--TEST--
Test Uri\Rfc3986\UriBuilder::setUserInfo() - success - empty string with base URI
--FILE--
<?php

$base = new Uri\Rfc3986\Uri('https://oldUser:oldPass@example.com:123/base/path?oldQuery#oldFragment');

$uri = new Uri\Rfc3986\UriBuilder()
    ->setUserInfo('')
    ->setHost('example.net')
    ->build($base);

var_dump($uri->toRawString());
var_dump($uri);
var_dump($uri->equals(new Uri\Rfc3986\Uri($uri->toRawString())));
var_dump($uri->equals(new Uri\Rfc3986\Uri('//@example.net', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(20) "https://@example.net"
object(Uri\Rfc3986\Uri)#%d (8) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  string(0) ""
  ["password"]=>
  string(0) ""
  ["host"]=>
  string(11) "example.net"
  ["port"]=>
  NULL
  ["path"]=>
  string(0) ""
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
bool(true)
bool(true)
