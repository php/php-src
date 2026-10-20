--TEST--
Test Uri\WhatWg\Url::resolve() - success - invalid URL unit warning
--FILE--
<?php

$errors = [];
$url1 = new Uri\WhatWg\Url("https://example.com");
$url2 = $url1->resolve(" /foo", $errors);

var_dump($url1->toAsciiString());

var_dump($url2);
var_dump($url2->toAsciiString());
var_dump($errors);

?>
--EXPECTF--
string(20) "https://example.com/"
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
  string(4) "/foo"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(23) "https://example.com/foo"
array(%d) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(5) " /foo"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
    ["failure"]=>
    bool(false)
  }
}
