--TEST--
Test Uri\WhatWg\UrlBuilder::build() - success - validation warning order with base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('https://example.com/base/path');

$errors = [];

$reference = new Uri\WhatWg\Url('//newUser:newPass@127.0.0.1.\newPath', $base);

$url = new Uri\WhatWg\UrlBuilder()
    ->setUsername('newUser')
    ->setPassword('newPass')
    ->setHost('127.0.0.1.')
    ->setPath('\newPath')
    ->build($base, $errors);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($errors);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals($reference, Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(41) "https://newUser:newPass@127.0.0.1/newPath"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  string(7) "newUser"
  ["password"]=>
  string(7) "newPass"
  ["host"]=>
  string(9) "127.0.0.1"
  ["port"]=>
  NULL
  ["path"]=>
  string(8) "/newPath"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
array(2) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(8) "\newPath"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidReverseSoldius)
    ["failure"]=>
    bool(false)
  }
  [1]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(0) ""
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::Ipv4EmptyPart)
    ["failure"]=>
    bool(false)
  }
}
bool(true)
bool(true)
