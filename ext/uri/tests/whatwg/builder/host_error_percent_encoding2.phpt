--TEST--
Test Uri\WhatWg\UrlBuilder::setHost() - error - invalid percent encoded octet in IPv6
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("https");
$builder->setHost("[2001:%308:85a3:0000:0000:8a2e:0370:7334]");

try {
    $builder->build();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified host is malformed (Ipv6InvalidCodePoint)
