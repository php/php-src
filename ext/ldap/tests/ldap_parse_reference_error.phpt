--TEST--
ldap_parse_reference() - ldap_parse_reference() operations that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require "connect.inc";
$link = ldap_connect($host, $port);
$refs = null;
var_dump(
	ldap_parse_reference($link, $link),
	ldap_parse_reference($link, $link, $refs),
	ldap_parse_reference($link, $refs, $refs, "Additional data"),
	$refs
);
?>
===DONE===
--EXPECTF--
Warning: Wrong parameter count for ldap_parse_reference() in %s on line %d

Warning: ldap_parse_reference(): supplied resource is not a valid ldap result entry resource in %s on line %d

Warning: Wrong parameter count for ldap_parse_reference() in %s on line %d
NULL
bool(false)
NULL
NULL
===DONE===
