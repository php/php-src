--TEST--
Test file_get_contents() function when a custom URI parser is configured
--FILE--
<?php

try {
    $context = stream_context_create([
        "http" => [
            "uri_parser_class" => "not-exists",
        ],
    ]);
    var_dump(file_get_contents("http://example.com", context: $context));
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

$context = stream_context_create([
    "http" => [
        "uri_parser_class" => null,
    ],
]);
var_dump(file_get_contents("http:///example.com", context: $context)); // invalid for parse_url only, valid for the other handlers

$context = stream_context_create([
    "http" => [
        "uri_parser_class" => \Uri\Rfc3986\Uri::class,
    ],
]);
var_dump(file_get_contents("http://éxamplé.com", context: $context));  // invalid for RFC 3986 only, valid for the other handlers

$context = stream_context_create([
    "http" => [
        "uri_parser_class" => \Uri\WhatWg\Url::class,
    ],
]);
var_dump(file_get_contents("http://exa%23mple.org", context: $context)); // invalid for WHATWG only, valid for the other handlers

?>
--EXPECTF--
file_get_contents(): Provided stream context has invalid value for the "uri_parser_class" option

Warning: file_get_contents(http:///example.com): Failed to open stream: operation failed in %s on line %d
bool(false)

Warning: file_get_contents(http://éxamplé.com): Failed to open stream: operation failed in %s on line %d
bool(false)

Warning: file_get_contents(http://exa%23mple.org): Failed to open stream: operation failed in %s on line %d
bool(false)
