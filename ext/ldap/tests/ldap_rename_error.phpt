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
var_dump(ldap_rename($link));
var_dump(ldap_rename($link, "cn=userNotFound,$base", "cn=userZ", "$base", true));
?>
===DONE===
--EXPECTF--
Warning: ldap_rename() expects at least 5 parameters, 1 given in %s on line %d
NULL
bool(false)
===DONE===
