--TEST--
Test Uri\WhatWg\Url parsing reports invalid URL unit warnings
--FILE--
<?php

$errors = [];
$url = new Uri\WhatWg\Url(" https://example.org ", null, $errors);

var_dump($url->toAsciiString());
var_dump($errors);

?>
--EXPECTF--
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
