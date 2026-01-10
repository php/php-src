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

$link = ldap_connect($uri);
var_dump($link);
?>
--EXPECTF--
object(LDAP\Connection)#%d (0) {
}
