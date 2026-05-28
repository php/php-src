--TEST--
Test Uri\WhatWg\UrlBuilder::setFragment() - error - contains Unicode character
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();

try {
    $builder->setFragment("főő");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--

