--TEST--
Test Uri\WhatWg\UrlBuilder::build() - error - empty query with an opaque base and ignored path characters
--FILE--
<?php

$base = new Uri\WhatWg\Url("foo:opaque?old#old");
$builder = new Uri\WhatWg\UrlBuilder();
$builder->setPath("\t\n");
$builder->setQuery("");
$builder->setFragment("new");

try {
    $builder->build($base);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
    var_dump($e->errors[0]->type);
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified query is malformed (MissingSchemeNonRelativeUrl)
enum(Uri\WhatWg\UrlValidationErrorType::MissingSchemeNonRelativeUrl)
