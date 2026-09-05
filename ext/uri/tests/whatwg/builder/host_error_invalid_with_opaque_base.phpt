--TEST--
Test Uri\WhatWg\UrlBuilder::setHost() - error - invalid host with base URL containing opaque path
--FILE--
<?php

$base = new Uri\WhatWg\Url('scheme:opaque?oldQuery#oldFragment');

$builder = new Uri\WhatWg\UrlBuilder()
    ->setHost('exa mple');

$referenceFailureType = null;

foreach ([
    fn() => new Uri\WhatWg\Url('//exa mple', $base),
    fn() => $builder->build($base),
] as $build) {
    try {
        $build();
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";

        if ($referenceFailureType === null) {
            $referenceFailureType = $e->errors[0]->type;
            continue;
        }

        var_dump($referenceFailureType === $e->errors[0]->type);
        var_dump($e->errors[0]->type);
    }
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URI is malformed (MissingSchemeNonRelativeUrl)
Uri\WhatWg\InvalidUrlException: The specified host is malformed (MissingSchemeNonRelativeUrl)
bool(true)
enum(Uri\WhatWg\UrlValidationErrorType::MissingSchemeNonRelativeUrl)
