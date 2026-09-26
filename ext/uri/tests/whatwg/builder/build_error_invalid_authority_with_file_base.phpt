--TEST--
Test Uri\WhatWg\UrlBuilder::build() - error - invalid authority with file base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('file:///base/path');

$builder = new Uri\WhatWg\UrlBuilder()
    ->setUsername('user')
    ->setPassword('pass')
    ->setHost('256.256.256.256')
    ->setPort(124);

$referenceErrors = null;

foreach ([
    fn() => new Uri\WhatWg\Url('//user:pass@256.256.256.256:124', $base),
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
    string(25) ":pass@256.256.256.256:124"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::DomainInvalidCodePoint)
    ["failure"]=>
    bool(true)
  }
}
