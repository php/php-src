--TEST--
Test Uri\WhatWg\UrlBuilder::setFragment() - success - trailing space with base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('https://example.com/base/path?oldQuery#oldFragment');

$errors = [];

$url = new Uri\WhatWg\UrlBuilder()
    ->setFragment('newFragment ')
    ->build($base, $errors);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($errors);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));
var_dump($url->equals(new Uri\WhatWg\Url('#newFragment%20', $base), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(53) "https://example.com/base/path?oldQuery#newFragment%20"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(10) "/base/path"
  ["query"]=>
  string(8) "oldQuery"
  ["fragment"]=>
  string(14) "newFragment%20"
}
array(1) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(1) " "
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
    ["failure"]=>
    bool(false)
  }
}
bool(true)
bool(true)
