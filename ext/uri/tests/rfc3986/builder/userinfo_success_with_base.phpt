--TEST--
Test Uri\Rfc3986\UriBuilder::setUserInfo() - success - with base URI
--FILE--
<?php

$base = new Uri\Rfc3986\Uri('https://user:pass@example.com:123/base/path?oldQuery#oldFragment');

$uri = new Uri\Rfc3986\UriBuilder()
    ->setUserInfo('newUser:newPassword')
    ->setHost('example.net')
    ->build($base);

var_dump($uri->toRawString());
var_dump($uri);
var_dump($uri->equals(new Uri\Rfc3986\Uri($uri->toRawString())));
var_dump($uri->equals(new Uri\Rfc3986\Uri('//newUser:newPassword@example.net', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(39) "https://newUser:newPassword@example.net"
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  string(7) "newUser"
  ["password"]=>
  string(11) "newPassword"
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
