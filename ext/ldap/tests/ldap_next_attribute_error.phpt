--TEST--
ldap_next_attribute() - Testing ldap_next_attribute() that should fail
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
$result = ldap_search($link, "dc=my-domain,dc=com", "(objectclass=organization)");
$entry = ldap_first_entry($link, $result);
var_dump(
	ldap_next_attribute($link),
	ldap_next_attribute($link, $link),
	ldap_next_attribute($link, $entry)
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
Warning: ldap_next_attribute() expects %s 2 parameters, 1 given in %s on line %d

Warning: ldap_next_attribute(): supplied resource is not a valid ldap result entry resource in %s on line %d

Warning: ldap_next_attribute(): called before calling ldap_first_attribute() or no attributes found in result entry in %s on line %d
NULL
bool(false)
bool(false)
===DONE===
