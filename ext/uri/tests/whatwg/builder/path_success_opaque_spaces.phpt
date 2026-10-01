--TEST--
Test Uri\WhatWg\UrlBuilder::setPath() - success - leading and trailing spaces in an opaque path
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("foo");
$builder->setPath("  abc ");
$softErrors = [];
$url = $builder->build(softErrors: $softErrors);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($softErrors);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
string(10) "foo:  abc "
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
  string(6) "  abc "
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
array(3) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(1) " "
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
    ["failure"]=>
    bool(false)
  }
  [1]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(5) " abc "
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
    ["failure"]=>
    bool(false)
  }
  [2]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(6) "  abc "
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
    ["failure"]=>
    bool(false)
  }
}
bool(false)
