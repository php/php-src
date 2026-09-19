--TEST--
Test Uri\WhatWg\UrlBuilder::setPassword() - success - invalid percent encoding with base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('https://example.com/base/path');

$errors = [];

$reference = new Uri\WhatWg\Url('//:%@example.net', $base);

$url = new Uri\WhatWg\UrlBuilder()
    ->setPassword('%')
    ->setHost('example.net')
    ->build($base, $errors);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($errors);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals($reference, Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(23) "https://:%@example.net/"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  string(0) ""
  ["password"]=>
  string(1) "%"
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
