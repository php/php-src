--TEST--
Test var_export
--EXTENSIONS--
uri
--FILE--
<?php

$uri = new Uri\Rfc3986Uri("https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");
var_export($uri);

$uri = new Uri\WhatWgUri("https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");
var_export($uri);

?>
--EXPECT--
\Uri\Rfc3986Uri::__set_state(array(
   'scheme' => 'https',
   'user' => 'username',
   'password' => 'password',
   'host' => 'www.google.com',
   'port' => 8080,
   'path' => 'pathname1/pathname2/pathname3',
   'query' => 'query=true',
   'fragment' => 'hash-exists',
))\Uri\WhatWgUri::__set_state(array(
   'scheme' => 'https',
   'user' => 'username',
   'password' => 'password',
   'host' => 'www.google.com',
   'port' => 8080,
   'path' => 'pathname1/pathname2/pathname3',
   'query' => 'query=true',
   'fragment' => 'hash-exists',
))
