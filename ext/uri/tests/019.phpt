--TEST--
Test IDNA support
--EXTENSIONS--
uri
--FILE--
<?php

$uri = \Uri\Uri::fromRfc3986("🐘");
var_dump($uri);

$errors = [];
$uri = \Uri\Uri::fromWhatWg("🐘", null, $errors);
var_dump($uri);
var_dump($errors[0]->errorCode === \Uri\WhatWgError::ERROR_TYPE_MISSING_SCHEME_NON_RELATIVE_URL);

?>
--EXPECT--
NULL
NULL
bool(true)
