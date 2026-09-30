--TEST--
Test Uri\WhatWg\UrlBuilder::setFragment() - success - strips a single leading hashmark
--FILE--
<?php

$errors = [];

$url = new Uri\WhatWg\UrlBuilder()
    ->setScheme('https')
    ->setHost('example.com')
    ->setFragment('##a')
    ->build(softErrors: $errors);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($errors);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString()), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(23) "https://example.com/##a"
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
  string(1) "/"
  ["query"]=>
  NULL
  ["fragment"]=>
  string(2) "#a"
}
array(1) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(2) "#a"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
    ["failure"]=>
    bool(false)
  }
}
bool(true)
