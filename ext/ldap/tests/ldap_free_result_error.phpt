--TEST--
ldap_free_result() - Testing ldap_free_result() that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect($host, $port);
var_dump(ldap_free_result($link));
var_dump(ldap_free_result($link, "Additional data"));
?>
===DONE===
--EXPECTF--
Warning: ldap_free_result(): supplied resource is not a valid ldap result resource in %s on line %d
bool(false)

Warning: ldap_free_result() expects exactly 1 parameter, 2 given in %s on line %d
NULL
===DONE===
