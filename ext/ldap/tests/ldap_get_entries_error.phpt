--TEST--
ldap_get_entries() - ldap_get_entries() operation that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect($host, $port);

// Too few parameters
var_dump(ldap_get_entries($link));

// Too many parameters
var_dump(ldap_get_entries($link, $link, "Additional data"));

// Bad parameter
var_dump(ldap_get_entries($link, "string"));
?>
===DONE===
--EXPECTF--
Warning: Wrong parameter count for ldap_get_entries() in %s on line %d
NULL

Warning: Wrong parameter count for ldap_get_entries() in %s on line %d
NULL

Warning: ldap_get_entries(): supplied argument is not a valid ldap result resource in %s on line %d
bool(false)
===DONE===
