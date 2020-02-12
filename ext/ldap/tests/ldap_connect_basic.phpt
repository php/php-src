--TEST--
ldap_connect() - Basic connection
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--EXTENSIONS--
ldap
--FILE--
<?php
require "connect.inc";

$link = ldap_connect($host, $port);
var_dump($link);
?>
--EXPECTF--
Deprecated: Usage of ldap_connect with two arguments is deprecated. in %s on line %d
object(LDAP\Connection)#%d (0) {
}