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
Warning: ldap_get_entries() expects exactly 2 parameters, 1 given in %s on line %d
NULL

Warning: ldap_get_entries() expects exactly 2 parameters, 3 given in %s on line %d
NULL

Warning: ldap_get_entries() expects parameter 2 to be resource, string given in %s on line %d
NULL
===DONE===
