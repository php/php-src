--TEST--
Test var_export
--EXTENSIONS--
uri
--FILE--
<?php

$uri = new Uri\Rfc3986\Uri("https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");
var_export($uri);
echo "\n";

$url = new Uri\WhatWg\Url("https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");
var_export($url);
echo "\n";

?>
--EXPECT--
\Uri\Rfc3986\Uri::__set_state(array(
))
\Uri\WhatWg\Url::__set_state(array(
))
