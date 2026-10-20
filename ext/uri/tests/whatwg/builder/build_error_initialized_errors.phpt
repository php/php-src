--TEST--
Test Uri\WhatWg\UrlBuilder::setHost() - error - initializes validation errors
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();

try {
    $builder->setHost("example.com/path");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
    var_dump($e->errors);
}

?>
--EXPECTF--
Uri\WhatWg\InvalidUrlException: The specified host is malformed (HostInvalidCodePoint)
array(1) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(0) ""
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::HostInvalidCodePoint)
    ["failure"]=>
    bool(true)
  }
}
