--TEST--
Test Uri\WhatWg\UrlBuilder::setPort() - error - missing host with base URL
--FILE--
<?php

$base = new Uri\WhatWg\Url('https://example.com');

$builder = new Uri\WhatWg\UrlBuilder()
    ->setPort(123);

try {
    $builder->build($base);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URL cannot have port
