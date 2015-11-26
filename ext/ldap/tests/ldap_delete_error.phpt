--TEST--
ldap_delete() - Delete operation that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifbindfailure.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);

// Too few parameters
var_dump(ldap_delete());
var_dump(ldap_delete($link));

// Too many parameters
var_dump(ldap_delete($link, "$base", "Additional data"));

// Invalid DN
var_dump(
	ldap_delete($link, "weirdAttribute=val"),
	ldap_error($link),
	ldap_errno($link)
);

// Deleting unexisting data
var_dump(
	ldap_delete($link, "dc=my-domain,$base"),
	ldap_error($link),
	ldap_errno($link)
);
?>
===DONE===
--CLEAN--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
?>
--EXPECTF--
Warning: ldap_delete() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: ldap_delete() expects exactly 2 parameters, 1 given in %s on line %d
NULL

Warning: ldap_delete() expects exactly 2 parameters, 3 given in %s on line %d
NULL

Warning: ldap_delete(): Delete: Invalid DN syntax in %s on line %d
bool(false)
string(17) "Invalid DN syntax"
int(34)

Warning: ldap_delete(): Delete: No such object in %s on line %d
bool(false)
string(14) "No such object"
int(32)
===DONE===
