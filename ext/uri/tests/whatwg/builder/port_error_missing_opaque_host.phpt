--TEST--
Test Uri\WhatWg\UrlBuilder::setPort() - error - missing opaque host
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("scheme");
$builder->setPort(443);

try {
    $builder->build();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URL cannot have port
