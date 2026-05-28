--TEST--
Test Uri\WhatWg\UrlBuilder basic - error - with base URL
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder()
    ->setPath("/foo/bar/baz");

try {
    $builder->build(new Uri\WhatWg\Url("/foo/bar"));
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified base URI must be absolute
