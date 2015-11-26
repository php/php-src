--TEST--
ldap_count_entries() - Testing counting LDAP entries that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect($host, $port);
var_dump(ldap_count_entries($link));
var_dump(ldap_count_entries($link, $link));
?>
===DONE===
--EXPECTF--
Warning: ldap_count_entries() expects exactly 2 parameters, 1 given in %s on line %d
NULL

Warning: ldap_count_entries(): supplied resource is not a valid ldap result resource in %s on line %d
bool(false)
===DONE===
