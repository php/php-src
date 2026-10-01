--TEST--
Test Uri\WhatWg\UrlBuilder::build() - error - preserves soft errors output with an opaque base URL
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setPath("/a\tb");
$softErrors = ["previous error"];

try {
    $builder->build(new Uri\WhatWg\Url("foo:opaque"), $softErrors);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
    foreach ($e->errors as $error) {
        var_dump($error->type);
    }
}

var_dump($softErrors);

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified path is malformed (MissingSchemeNonRelativeUrl)
enum(Uri\WhatWg\UrlValidationErrorType::MissingSchemeNonRelativeUrl)
enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
array(0) {
}
