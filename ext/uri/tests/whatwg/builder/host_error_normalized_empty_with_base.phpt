--TEST--
Test Uri\WhatWg\UrlBuilder::build() - error - rejects credentials after host normalization with a base URL
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setHost("\t");
$builder->setUsername("user");

try {
    $builder->build(new Uri\WhatWg\Url("foo://example.com/"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
    foreach ($e->errors as $error) {
        var_dump($error->type);
    }
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URL cannot have username
enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
