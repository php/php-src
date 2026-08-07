--TEST--
Test Uri\WhatWg\Url component modification - error - modifying multiple components with warnings before throwing an exception
--FILE--
<?php

$url = new Uri\WhatWg\Url("https://example.com")
    ->withScheme("\tscheme")
    ->withHost("\tex.com")
    ->withQuery("\refoo=bar")
    ->withFragment("\nfoo");

try {
    $url->withScheme("0");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    var_dump($e->errors);
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified scheme is malformed
array(0) {
}
