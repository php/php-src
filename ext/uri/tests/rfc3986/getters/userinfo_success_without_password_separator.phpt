--TEST--
Test Uri\Rfc3986\Uri component retrieval - userinfo - without password separator
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
