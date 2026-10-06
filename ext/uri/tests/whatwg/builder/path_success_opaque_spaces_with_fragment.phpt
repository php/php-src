--TEST--
Test Uri\WhatWg\UrlBuilder::setPath() - success - trailing spaces before a fragment
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("foo");
$builder->setPath("abc  ");
$builder->setFragment("f");
$softErrors = [];
$url = $builder->build(softErrors: $softErrors);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($softErrors);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString()), Uri\UriComparisonMode::IncludeFragment));

?>
--EXPECTF--
string(13) "foo:abc %20#f"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(3) "foo"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(7) "abc %20"
  ["query"]=>
  NULL
  ["fragment"]=>
  string(1) "f"
}
array(2) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(2) " #"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
    ["failure"]=>
    bool(false)
  }
  [1]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(3) "  #"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
    ["failure"]=>
    bool(false)
  }
}
bool(true)
