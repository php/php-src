--TEST--
Test Uri\WhatWg\UrlBuilder::setPath() - error - with base URL containing opaque path
--FILE--
<?php

$base = new Uri\WhatWg\Url('scheme:opaque-path');

$builder = new Uri\WhatWg\UrlBuilder()
    ->setPath('/foo/bar/baz');

$referenceErrors = null;

foreach ([
    fn() => new Uri\WhatWg\Url('/foo/bar/baz', $base),
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
Uri\WhatWg\InvalidUrlException: The specified URI is malformed (MissingSchemeNonRelativeUrl)
Uri\WhatWg\InvalidUrlException: The specified path is malformed (MissingSchemeNonRelativeUrl)
bool(true)
array(1) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (3) {
    ["context"]=>
    string(12) "/foo/bar/baz"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::MissingSchemeNonRelativeUrl)
    ["failure"]=>
    bool(true)
  }
}
