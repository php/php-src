--TEST--
Test Uri\WhatWg\UrlBuilder::build() - success - collects and resets soft errors with a base URL
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setPath("/a\tb");
$builder->setFragment("x\ny");
$base = new Uri\WhatWg\Url("https://example.com/");
$softErrors = ["previous error"];
$url = $builder->build($base, $softErrors);

var_dump($url->toAsciiString());
var_dump($softErrors);

$builder->setPath("/ab");
$builder->setFragment("xy");
$builder->build($base, $softErrors);
var_dump($softErrors);

?>
--EXPECTF--
string(25) "https://example.com/ab#xy"
array(2) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(2) "	b"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
    ["failure"]=>
    bool(false)
  }
  [1]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(2) "
y"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
    ["failure"]=>
    bool(false)
  }
}
array(0) {
}
