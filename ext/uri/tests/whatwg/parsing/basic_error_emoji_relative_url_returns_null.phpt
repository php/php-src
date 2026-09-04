--TEST--
Test Uri\WhatWg\Url parsing - basic - parse() returns null for an emoji relative-URL
--FILE--
<?php

$errors = [];
$url = Uri\WhatWg\Url::parse("🐘", null, $errors);

var_dump($url);
var_dump($errors);

?>
--EXPECTF--
NULL
array(1) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(4) "🐘"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::MissingSchemeNonRelativeUrl)
    ["failure"]=>
    bool(true)
  }
}
