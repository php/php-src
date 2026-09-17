--TEST--
Test Uri\Rfc3986\Uri array cast - success
--FILE--
<?php

$uri = new Uri\Rfc3986\Uri("https://username:password@www.example.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");
var_dump((array) $uri);

?>
--EXPECTF--
array(%d) {
}
