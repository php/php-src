--TEST--
Test Uri\WhatWg\UrlBuilder::setUsername() - error - with file base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('file:///Users/example/base/path');

$builder = new Uri\WhatWg\UrlBuilder()
    ->setUsername('newUsername')
    ->setHost('example.net');

$referenceErrors = null;

foreach ([
    fn() => new Uri\WhatWg\Url('//newUsername@example.net', $base),
    fn() => $builder->build($base),
] as $build) {
    try {
        $build();
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";

        if ($referenceErrors === null) {
            $referenceErrors = $e->errors;
            continue;
        }

        var_dump($referenceErrors == $e->errors);
        var_dump($e->errors);
    }
}

?>
--EXPECTF--
Uri\WhatWg\InvalidUrlException: The specified URI is malformed (DomainInvalidCodePoint)
Uri\WhatWg\InvalidUrlException: The specified username is malformed (DomainInvalidCodePoint)
bool(true)
array(1) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (3) {
    ["context"]=>
    string(12) "@example.net"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::DomainInvalidCodePoint)
    ["failure"]=>
    bool(true)
  }
}
