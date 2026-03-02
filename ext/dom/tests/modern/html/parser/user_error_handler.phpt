--TEST--
Parse HTML document with user error handler and error_reporting(0)
--EXTENSIONS--
dom
--INI--
error_reporting=0
--FILE--
<?php

set_error_handler(function ($errno, $errstr, $errfile, $errline) {
    var_dump($errno, $errstr);
}, E_WARNING);

Dom\HTMLDocument::createFromString('<html></html>');

?>
--EXPECT--
int(2)
string(113) "Dom\HTMLDocument::createFromString(): tree error unexpected-token-in-initial-mode in Entity, line: 1, column: 2-5"
