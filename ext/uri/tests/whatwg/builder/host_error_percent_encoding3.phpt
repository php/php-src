--TEST--
Test Uri\WhatWg\UrlBuilder::setHost() - error - invalid percent encoded octet in IPv4
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("https");
$builder->setHost("192.168.%8.1");

try {
    $builder->build();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified host is malformed (DomainInvalidCodePoint)
