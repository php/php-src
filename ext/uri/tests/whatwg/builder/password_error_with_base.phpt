--TEST--
Test Uri\WhatWg\UrlBuilder::setPassword() - error - missing opaque host with base URL
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setPassword("password");

try {
    $builder->build(new Uri\WhatWg\Url("https://example.com"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URL cannot have password
