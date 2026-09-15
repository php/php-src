--TEST--
Test Uri\WhatWg\UrlBuilder::build() - success - all components with non-special base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('scheme://oldUser:oldPass@example.com:123/base/path?oldQuery#oldFragment');

$url = new Uri\WhatWg\UrlBuilder()
    ->setUsername('newUser')
    ->setPassword('newPass')
    ->setHost('example.net')
    ->setPort(124)
    ->setPath('newPath')
    ->setQuery('newQuery')
    ->setFragment('newFragment')
    ->build($base);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals(new Uri\WhatWg\Url('//newUser:newPass@example.net:124/newPath?newQuery#newFragment', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(69) "scheme://newUser:newPass@example.net:124/newPath?newQuery#newFragment"
object(Uri\WhatWg\Url)#%d (8) {
  ["scheme"]=>
  string(6) "scheme"
  ["username"]=>
  string(7) "newUser"
  ["password"]=>
  string(7) "newPass"
  ["host"]=>
  string(11) "example.net"
  ["port"]=>
  int(124)
  ["path"]=>
  string(8) "/newPath"
  ["query"]=>
  string(8) "newQuery"
  ["fragment"]=>
  string(11) "newFragment"
}
bool(true)
bool(true)
