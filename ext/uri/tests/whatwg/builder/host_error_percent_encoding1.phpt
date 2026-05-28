--TEST--
Test Uri\WhatWg\UrlBuilder::setHost() - error - invalid percent encoding octet in registered name
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("https");
$builder->setHost("ex%3mple.co");

try {
    $builder->build();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified host is malformed (DomainInvalidCodePoint)
