--TEST--
php_sapi_name() function when switching to webserver by using post 
--POST--
foo=BAR
--CREDITS--
Sebastian Schürmann
sebs@php.net
Testfest 2009 Munich
--FILE--
<?php
var_dump(php_sapi_name());
?>
--EXPECT--
string(8) "cgi-fcgi"
