--TEST--
Test Uri\WhatWg\UrlBuilder::build() - success - clears soft errors from a previous build
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("https");
$builder->setHost("example.com");
$builder->setFragment("a\tb");
$softErrors = [];
$builder->build(softErrors: $softErrors);

var_dump($softErrors);

$builder->setFragment("ab");
$url = $builder->build(softErrors: $softErrors);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($softErrors);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
array(1) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(2) "	b"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
    ["failure"]=>
    bool(false)
  }
}
string(23) "https://example.com/#ab"
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
  string(2) "ab"
}
array(0) {
}
bool(true)
