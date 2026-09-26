--TEST--
Test Uri\WhatWg\Url parsing - basic - validation error order
--XFAIL--
validation errors should be returned in parser order
--FILE--
<?php

$errors = [];
$url = new Uri\WhatWg\Url('https://127.0.0.1.\path', softErrors: $errors);

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
  string(9) "127.0.0.1"
  ["port"]=>
  NULL
  ["path"]=>
  string(5) "/path"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(22) "https://127.0.0.1/path"
array(2) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(0) ""
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::Ipv4EmptyPart)
    ["failure"]=>
    bool(false)
  }
  [1]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(5) "\path"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidReverseSoldius)
    ["failure"]=>
    bool(false)
  }
}
