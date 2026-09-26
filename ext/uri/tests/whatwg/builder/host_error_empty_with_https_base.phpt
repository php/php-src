--TEST--
Test Uri\WhatWg\UrlBuilder::setHost() - error - empty host with HTTPS base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('https://example.com/base/path');

$builder = new Uri\WhatWg\UrlBuilder()
    ->setHost('');

$referenceErrors = null;

foreach ([
    fn() => new Uri\WhatWg\Url('//', $base),
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
Uri\WhatWg\InvalidUrlException: The specified URI is malformed (HostMissing)
Uri\WhatWg\InvalidUrlException: The specified host is malformed (HostMissing)
bool(true)
array(1) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (3) {
    ["context"]=>
    string(0) ""
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::HostMissing)
    ["failure"]=>
    bool(true)
  }
}
