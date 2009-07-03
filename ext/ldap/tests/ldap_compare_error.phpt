--TEST--
ldap_compare() - Testing ldap_compare() that should fail
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
insert_dummy_data($link);

// Too few parameters
var_dump(ldap_compare($link));
var_dump(ldap_compare($link, $link));
var_dump(ldap_compare($link, $link, $link));

// Too many parameters
var_dump(ldap_compare($link, $link, $link, $link, "Additional data"));

var_dump(
	ldap_compare($link, "cn=userNotAvailable,dc=my-domain,dc=com", "sn", "testSN1"),
	ldap_error($link),
	ldap_errno($link)
);
?>
===DONE===
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
remove_dummy_data($link);
?>
--EXPECTF--
Warning: ldap_compare() expects exactly 4 parameters, 1 given in %s on line %d
NULL

Warning: ldap_compare() expects exactly 4 parameters, 2 given in %s on line %d
NULL

Warning: ldap_compare() expects exactly 4 parameters, 3 given in %s on line %d
NULL

Warning: ldap_compare() expects exactly 4 parameters, 5 given in %s on line %d
NULL

Warning: ldap_compare(): Compare: No such object in %s on line %d
int(-1)
string(14) "No such object"
int(32)
===DONE===
