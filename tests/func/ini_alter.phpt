--TEST--
ini_alter() check
--CREDITS--
Sebastian Schürmann
sebs@php.net
Testfest 2009 Munich
--FILE--
<?php
ini_alter('error_reporting', 1);
$var = ini_get('error_reporting');
var_dump($var);
ini_alter('error_reporting', 0);
$var = ini_get('error_reporting');
var_dump($var);
?>
--EXPECT--
unicode(1) "1"
unicode(1) "0"

