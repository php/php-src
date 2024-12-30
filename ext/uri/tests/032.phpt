--TEST--
Test JSON encoding
--EXTENSIONS--
uri
--FILE--
<?php

$uri = new Uri\Rfc3986\Uri("https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");
var_dump(json_encode($uri));

$uri = new Uri\WhatWg\Url("https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");
var_dump(json_encode($uri));

?>
--EXPECT--
string(181) "{"scheme":"https","user":"username","password":"password","host":"www.google.com","port":8080,"path":"pathname1\/pathname2\/pathname3","query":"query=true","fragment":"hash-exists"}"
string(181) "{"scheme":"https","user":"username","password":"password","host":"www.google.com","port":8080,"path":"pathname1\/pathname2\/pathname3","query":"query=true","fragment":"hash-exists"}"
