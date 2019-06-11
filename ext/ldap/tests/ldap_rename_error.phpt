--TEST--
ldap_rename() - Testing ldap_rename() that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect($host, $port);
var_dump(ldap_rename($link, "cn=userNotFound,$base", "cn=userZ", "$base", true));
?>
--EXPECT--
bool(false)
