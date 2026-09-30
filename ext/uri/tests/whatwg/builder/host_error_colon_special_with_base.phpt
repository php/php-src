--TEST--
Test Uri\WhatWg\UrlBuilder::setHost() - error - colon in special host input with base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('https://example.com/base/path');

$builder = new Uri\WhatWg\UrlBuilder()
    ->setHost('example.net:123');

try {
    $builder->build($base);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified host is malformed
