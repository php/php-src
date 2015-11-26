--TEST--
ldap_next_entry() - Testing ldap_next_entry() that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect($host, $port);
var_dump(ldap_next_entry($link));
var_dump(ldap_next_entry($link, $link, "Additional data"));
var_dump(ldap_next_entry($link, $link));
?>
===DONE===
--EXPECTF--
Warning: ldap_next_entry() expects exactly 2 parameters, 1 given in %s on line %d
NULL

Warning: ldap_next_entry() expects exactly 2 parameters, 3 given in %s on line %d
NULL

Warning: ldap_next_entry(): supplied resource is not a valid ldap result entry resource in %s on line %d
bool(false)
===DONE===
