--TEST--
Test Uri\Rfc3986\Uri userinfo without a password separator
--FILE--
<?php

$uri = new Uri\Rfc3986\Uri('https://user@example.com');

var_dump($uri->getUserInfo());
var_dump($uri->getUsername());
var_dump($uri->getPassword());

?>
--EXPECT--
string(4) "user"
string(4) "user"
string(0) ""
