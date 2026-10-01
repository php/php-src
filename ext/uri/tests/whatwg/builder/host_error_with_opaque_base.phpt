--TEST--
Test Uri\WhatWg\UrlBuilder::build() - error - new authority with an opaque base
--FILE--
<?php

$base = new Uri\WhatWg\Url("foo:opaque?old#old");
$builder = new Uri\WhatWg\UrlBuilder();
$builder->setHost("example.com");

try {
    $builder->build($base);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
    var_dump($e->errors[0]->type);
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified host is malformed (MissingSchemeNonRelativeUrl)
enum(Uri\WhatWg\UrlValidationErrorType::MissingSchemeNonRelativeUrl)
