--TEST--
Test Uri\WhatWg\UrlBuilder::setHost() - success - null in case of special URLs
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("https");
$builder->setHost(null);

try {
    $builder->build();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified host is malformed (HostMissing)
