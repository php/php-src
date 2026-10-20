--TEST--
Test Uri\WhatWg\Url::__construct() - error - clears soft errors when an exception is thrown
--FILE--
<?php

$softErrors = ["previous error"];

try {
    new Uri\WhatWg\Url("🐘", softErrors: $softErrors);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
    var_dump($e->errors);
}

var_dump($softErrors);

?>
--EXPECTF--
Uri\WhatWg\InvalidUrlException: The specified URI is malformed (MissingSchemeNonRelativeUrl)
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
array(0) {
}
