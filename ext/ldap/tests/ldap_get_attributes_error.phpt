--TEST--
ldap_get_attributes() - Testing ldap_get_attributes() that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect($host, $port);
var_dump(ldap_get_attributes($link));
var_dump(ldap_get_attributes($link, $link));
?>
===DONE===
--EXPECTF--
Warning: Wrong parameter count for ldap_get_attributes() in %s on line %d
NULL

Warning: ldap_get_attributes(): supplied resource is not a valid ldap result entry resource in %s on line %d
bool(false)
===DONE===
