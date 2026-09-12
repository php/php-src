--TEST--
Test Uri\WhatWg\UrlBuilder::setPort() - error - missing opaque host with base URL
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setPort(123);

try {
    $builder->build(new Uri\WhatWg\Url("https://example.com"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URL cannot have port
