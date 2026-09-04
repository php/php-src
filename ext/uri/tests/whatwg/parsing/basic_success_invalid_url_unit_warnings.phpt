--TEST--
Test Uri\WhatWg\Url parsing - basic - invalid URL unit warnings
--FILE--
<?php

$errors = [];
$url = new Uri\WhatWg\Url(" https://example.org ", null, $errors);

var_dump($url);
var_dump($url->toAsciiString());
var_dump($errors);

?>
--EXPECTF--
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.org"
  ["port"]=>
  NULL
  ["path"]=>
  string(1) "/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(20) "https://example.org/"
array(2) {
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
    string(21) " https://example.org "
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
    ["failure"]=>
    bool(false)
  }
}
