--TEST--
Test Uri\WhatWg\UrlBuilder::setHost() - error - IPv4 out-of-range part with file base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('file:///base/path');

$builder = new Uri\WhatWg\UrlBuilder()
    ->setHost('256.256.256.256');

$referenceErrors = null;

foreach ([
    fn() => new Uri\WhatWg\Url('//256.256.256.256', $base),
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
Uri\WhatWg\InvalidUrlException: The specified URI is malformed (Ipv4OutOfRangePart)
Uri\WhatWg\InvalidUrlException: The specified host is malformed (Ipv4OutOfRangePart)
bool(true)
array(4) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (3) {
    ["context"]=>
    string(3) "256"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::Ipv4OutOfRangePart)
    ["failure"]=>
    bool(true)
  }
  [1]=>
  object(Uri\WhatWg\UrlValidationError)#%d (3) {
    ["context"]=>
    string(7) "256.256"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::Ipv4OutOfRangePart)
    ["failure"]=>
    bool(true)
  }
  [2]=>
  object(Uri\WhatWg\UrlValidationError)#%d (3) {
    ["context"]=>
    string(11) "256.256.256"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::Ipv4OutOfRangePart)
    ["failure"]=>
    bool(true)
  }
  [3]=>
  object(Uri\WhatWg\UrlValidationError)#%d (3) {
    ["context"]=>
    string(15) "256.256.256.256"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::Ipv4OutOfRangePart)
    ["failure"]=>
    bool(true)
  }
}
