--TEST--
Test var_export() with Uri\Rfc3986\Uri
--FILE--
<?php

$uri = new Uri\Rfc3986\Uri("https://username:password@www.example.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");
var_export($uri);
echo "\n";

?>
--EXPECT--
\Uri\Rfc3986\Uri::__set_state(array(
))
