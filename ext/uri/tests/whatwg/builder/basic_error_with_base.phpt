--TEST--
Test Uri\WhatWg\UrlBuilder basic - error - with base URL
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setPath("/foo/bar/baz");

try {
    $builder->build(new Uri\WhatWg\Url("/foo/bar"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URI is malformed (MissingSchemeNonRelativeUrl)
