--TEST--
Test Uri\WhatWg\UrlBuilder::build() - error - empty reference with an opaque base
--FILE--
<?php

$base = new Uri\WhatWg\Url("foo:opaque?old#old");
$builder = new Uri\WhatWg\UrlBuilder();

try {
    $builder->build($base);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
    var_dump($e->errors[0]->type);
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified path is malformed (MissingSchemeNonRelativeUrl)
enum(Uri\WhatWg\UrlValidationErrorType::MissingSchemeNonRelativeUrl)
