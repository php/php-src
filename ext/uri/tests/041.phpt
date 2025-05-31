--TEST--
Test relative URI parsing
--EXTENSIONS--
uri
--FILE--
<?php

$errors = [];
$url = Uri\WhatWg\Url::parse("?query#fragment", null, $errors);
var_dump($url);
var_dump($errors);

?>
--EXPECTF--
NULL
array(%d) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(15) "?query#fragment"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::MissingSchemeNonRelativeUrl)
    ["failure"]=>
    bool(true)
  }
}
