--TEST--
Test IDNA support
--EXTENSIONS--
uri
--FILE--
<?php

$uri = Uri\Rfc3986Uri::create("🐘");
var_dump($uri);

$errors = [];
$uri = Uri\WhatWgUri::create("🐘", null, $errors);
var_dump($uri);
var_dump($errors[0]->errorCode === \Uri\WhatWgError::ERROR_TYPE_MISSING_SCHEME_NON_RELATIVE_URL);

?>
--EXPECT--
NULL
NULL
bool(true)
