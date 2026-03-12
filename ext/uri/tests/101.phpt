--TEST--
Test the Lexbor-based URI parser
--EXTENSIONS--
uri
zend_test
--FILE--
<?php

try {
  var_dump(zend_test_uri_parser('invalid uri', "Uri\\WhatWg\\Url"));
} catch (\Uri\WhatWg\InvalidUrlException $e) {
  echo $e->getMessage(), PHP_EOL;
  var_dump($e->errors);
}

?>
--EXPECTF--
The specified URI is malformed (MissingSchemeNonRelativeUrl)
array(1) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (3) {
    ["context"]=>
    string(11) "invalid uri"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::MissingSchemeNonRelativeUrl)
    ["failure"]=>
    bool(true)
  }
}
