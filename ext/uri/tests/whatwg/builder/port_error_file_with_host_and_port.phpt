--TEST--
Test Uri\WhatWg\UrlBuilder::setPort() - error - file scheme with host and port
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("file");
$builder->setHost("example.com");
$builder->setPort(123);

try {
    $builder->build();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URI is malformed
