--TEST--
Test Uri\WhatWg\UrlBuilder::setUsername() - success - special characters with base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('https://example.com/base/path');

$errors = [];

$reference = new Uri\WhatWg\Url('//~%%23@example.net', $base);

$url = new Uri\WhatWg\UrlBuilder()
    ->setUsername('~%#')
    ->setHost('example.net')
    ->build($base, $errors);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($errors);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals($reference, Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(26) "https://~%%23@example.net/"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  string(5) "~%%23"
  ["password"]=>
  string(0) ""
  ["host"]=>
  string(11) "example.net"
  ["port"]=>
  NULL
  ["path"]=>
  string(1) "/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
array(0) {
}
bool(true)
bool(true)
