--TEST--
Test Uri\Rfc3986\UriBuilder::setUserInfo() - success - with empty host and base URI
--FILE--
<?php

$base = new Uri\Rfc3986\Uri('https://oldUser:oldPass@example.com:123/base/path?oldQuery#oldFragment');

$uri = new Uri\Rfc3986\UriBuilder()
    ->setUserInfo('newUser:newPass')
    ->setHost('')
    ->build($base);

var_dump($uri->toRawString());
var_dump($uri);
var_dump($uri->equals(new Uri\Rfc3986\Uri($uri->toRawString())));
var_dump($uri->equals(new Uri\Rfc3986\Uri('//newUser:newPass@', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(24) "https://newUser:newPass@"
object(Uri\Rfc3986\Uri)#%d (8) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  string(7) "newUser"
  ["password"]=>
  string(7) "newPass"
  ["host"]=>
  string(0) ""
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
