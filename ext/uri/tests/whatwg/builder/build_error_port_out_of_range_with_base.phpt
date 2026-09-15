--TEST--
Test Uri\WhatWg\UrlBuilder::build() - error - port out of range with base URL
--EXTENSIONS--
reflection
--FILE--
<?php

$base = new Uri\WhatWg\Url('https://example.com/base/path');

$builder = new Uri\WhatWg\UrlBuilder()
    ->setHost('example.net');

new ReflectionProperty($builder, 'port')->setValue($builder, PHP_INT_MAX);

$referenceErrors = null;

foreach ([
    fn() => new Uri\WhatWg\Url('//example.net:' . PHP_INT_MAX, $base),
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
Uri\WhatWg\InvalidUrlException: The specified URI is malformed (PortOutOfRange)
Uri\WhatWg\InvalidUrlException: The specified port is malformed (PortOutOfRange)
bool(true)
array(1) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (3) {
    ["context"]=>
    string(0) ""
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::PortOutOfRange)
    ["failure"]=>
    bool(true)
  }
}
