--TEST--
Test Uri\WhatWg\UrlBuilder::setUsername() - error - empty string with Windows drive letter host and file base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('file:///base/path');

$builder = new Uri\WhatWg\UrlBuilder()
    ->setUsername('')
    ->setHost('C:')
    ->setPath('/newPath');

try {
    $builder->build($base);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
    var_dump($e->errors);
}

?>
--EXPECTF--
Uri\WhatWg\InvalidUrlException: The specified host is malformed (DomainInvalidCodePoint)
array(1) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (3) {
    ["context"]=>
    string(1) ":"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::DomainInvalidCodePoint)
    ["failure"]=>
    bool(true)
  }
}
