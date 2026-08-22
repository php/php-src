--TEST--
Test zend_test_uri_parser() with an invalid Uri\WhatWg\Url
--EXTENSIONS--
zend_test
--FILE--
<?php

try {
    var_dump(zend_test_uri_parser('invalid uri', "Uri\\WhatWg\\Url"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
    var_dump($e->errors);
}

?>
--EXPECTF--
Uri\WhatWg\InvalidUrlException: The specified URI is malformed (MissingSchemeNonRelativeUrl)
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
