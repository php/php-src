--TEST--
CGI with auto_globals_jit=0
--INI--
auto_globals_jit=0
--CGI--
--ENV--
FOO=BAR
--FILE--
<?php
var_dump($_SERVER['FOO']);
var_dump($_ENV['FOO']);
var_dump(getenv('FOO'));
?>
--EXPECT--
string(3) "BAR"
string(3) "BAR"
string(3) "BAR"
