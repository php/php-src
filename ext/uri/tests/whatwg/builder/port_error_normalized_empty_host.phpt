--TEST--
Test Uri\WhatWg\UrlBuilder::setPort() - error - host normalizes to empty
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder()
    ->setScheme('foo')
    ->setHost("\t\n")
    ->setPort(123);

try {
    $builder->build();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
    var_dump($e->errors);
}

?>
--EXPECTF--
Uri\WhatWg\InvalidUrlException: The specified URL cannot have port
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
