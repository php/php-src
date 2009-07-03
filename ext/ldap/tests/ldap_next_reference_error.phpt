--TEST--
ldap_next_reference() - Testing ldap_next_reference() that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect($host, $port);
var_dump(ldap_next_reference($link));
var_dump(ldap_next_reference($link, $link, "Additional data"));
var_dump(ldap_next_reference($link, $link));
?>
===DONE===
--EXPECTF--
Warning: Wrong parameter count for ldap_next_reference() in %s on line %d
NULL

Warning: Wrong parameter count for ldap_next_reference() in %s on line %d
NULL

Warning: ldap_next_reference(): supplied resource is not a valid ldap result entry resource in %s on line %d
bool(false)
===DONE===
