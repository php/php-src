--TEST--
Test Uri\WhatWg\UrlBuilder::setPassword() - error - host normalizes to empty with base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('foo://example.com/basePath');

$builder = new Uri\WhatWg\UrlBuilder()
    ->setHost("\t\n")
    ->setPassword('password');

try {
    $builder->build($base);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
    var_dump($e->errors);
}

?>
--EXPECTF--
Uri\WhatWg\InvalidUrlException: The specified URL cannot have password
array(1) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(2) "%r\x09\x0A%r"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
    ["failure"]=>
    bool(false)
  }
}
