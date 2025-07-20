--TEST--
Test file_get_contents() function when a custom URI parser is configured
--FILE--
<?php

$context = stream_context_create([
    "http" => [
        "uri_parser_class" => null,
    ],
]);
var_dump(file_get_contents("https:///example.com", context: $context)); // invalid for parse_url only, valid for the other handlers

$context = stream_context_create([
    "http" => [
        "uri_parser_class" => \Uri\Rfc3986\Uri::class,
    ],
]);
var_dump(file_get_contents("https://éxamplé.com", context: $context));  // invalid for RFC 3986 only, valid for the other handlers

$context = stream_context_create([
    "http" => [
        "uri_parser_class" => \Uri\WhatWg\Url::class,
    ],
]);
var_dump(file_get_contents("https://exa%23mple.org", context: $context)); // invalid for WHATWG only, valid for the other handlers

?>
--EXPECTF--
Warning: file_get_contents(https:///example.com): Failed to open stream: operation failed in %s on line %d
bool(false)

Warning: file_get_contents(https://éxamplé.com): Failed to open stream: operation failed in %s on line %d
bool(false)

Warning: file_get_contents(https://exa%23mple.org): Failed to open stream: operation failed in %s on line %d
bool(false)
