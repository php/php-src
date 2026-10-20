--TEST--
Test Uri\WhatWg\UrlBuilder::setPath() - success - trailing spaces with empty query and fragment inputs
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("foo");
$builder->setPath("abc ");
$builder->setQuery("");
$builder->setFragment("");
$url = $builder->build(softErrors: $softErrors);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($softErrors);

?>
--EXPECTF--
string(8) "foo:abc "
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
  string(4) "abc "
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
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
