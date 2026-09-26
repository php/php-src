--TEST--
Test Uri\WhatWg\UrlBuilder::setPort() - error - with file base URL and empty username
--FILE--
<?php

$base = new Uri\WhatWg\Url('file:///base/path');

$builder = new Uri\WhatWg\UrlBuilder()
    ->setUsername('')
    ->setHost('example.net')
    ->setPort(124);

$referenceErrors = null;

foreach ([
    fn() => new Uri\WhatWg\Url('//example.net:124', $base),
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
Uri\WhatWg\InvalidUrlException: The specified port is malformed (DomainInvalidCodePoint)
bool(true)
array(1) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (3) {
    ["context"]=>
    string(4) ":124"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::DomainInvalidCodePoint)
    ["failure"]=>
    bool(true)
  }
}
