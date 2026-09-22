--TEST--
Test Uri\Rfc3986\Uri json_encode() - success
--FILE--
<?php

$uri = new Uri\Rfc3986\Uri("https://username:password@www.example.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");
var_dump(json_encode($uri));

?>
--EXPECT--
string(2) "{}"
