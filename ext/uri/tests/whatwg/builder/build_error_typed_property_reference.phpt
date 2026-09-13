--TEST--
Test Uri\WhatWg\UrlBuilder::build() - error - soft errors assigned to typed property reference
--FILE--
<?php

class Errors
{
    public string $value = '';
}

$errors = new Errors();

try {
    new Uri\WhatWg\UrlBuilder()
        ->setScheme('https')
        ->setHost('127.0.0.1.')
        ->build(softErrors: $errors->value);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: Cannot assign array to reference held by property Errors::$value of type string
