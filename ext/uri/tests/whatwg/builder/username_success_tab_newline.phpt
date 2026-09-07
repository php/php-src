--TEST--
Test Uri\WhatWg\UrlBuilder::setUsername() - success - contains tab and newline characters
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("\tfo\no");
$builder->setHost("example.com");
$builder->setUsername("f\no\ro\t");
$errors = [];
$url = $builder->build(errors: $errors);

var_dump($url->toAsciiString());
var_dump($url);
var_dump($errors);
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECT--
string(30) "foo://f%0Ao%0Do%09@example.com"
object(Uri\WhatWg\Url)#4 (8) {
  ["scheme"]=>
  string(3) "foo"
  ["username"]=>
  string(12) "f%0Ao%0Do%09"
  ["password"]=>
  string(0) ""
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(0) ""
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
array(1) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#2 (3) {
    ["context"]=>
    string(5) "	fo
o"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
    ["failure"]=>
    bool(false)
  }
}
bool(true)
