--TEST--
ldap_connect() - With two parameters is marked as deprecated
--CREDITS--
Andreas Heigl <heiglandreas@php.net>
--FILE--
<?php
require "connect.inc";

ldap_connect('foobar', 123);
?>
--EXPECTF--
Deprecated: Usage of ldap_connect with two arguments is deprecated. in %s on line %d
