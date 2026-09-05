--TEST--
Test Uri\WhatWg\UrlBuilder basic - error - with base URL containing opaque path
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setPath("/foo/bar/baz");

try {
    $builder->build(new Uri\WhatWg\Url("scheme:opaque-path"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified path is malformed (MissingSchemeNonRelativeUrl)
