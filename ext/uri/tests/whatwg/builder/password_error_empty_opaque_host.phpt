--TEST--
Test Uri\WhatWg\UrlBuilder::setPassword() - error - empty opaque host
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("scheme");
$builder->setHost("");
$builder->setPassword("password");

try {
    $builder->build();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URL cannot have password
