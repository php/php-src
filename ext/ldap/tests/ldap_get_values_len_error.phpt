--TEST--
ldap_get_values_len() - Testing ldap_get_values_len() that should fail
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

// Too few parameters
var_dump(ldap_get_values_len($link));
var_dump(ldap_get_values_len($link, $entry));
var_dump(ldap_get_values_len($link, $entry, "weirdAttribute", "Additional data"));

var_dump(ldap_get_values_len($link, $entry, "inexistantAttribute"));
?>
===DONE===
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
remove_dummy_data($link);
?>
--EXPECTF--
Warning: ldap_get_values_len() expects exactly 3 parameters, 1 given in %s on line %d
NULL

Warning: ldap_get_values_len() expects exactly 3 parameters, 2 given in %s on line %d
NULL

Warning: ldap_get_values_len() expects exactly 3 parameters, 4 given in %s on line %d
NULL

Warning: ldap_get_values_len(): Cannot get the value(s) of attribute %s in %s on line %d
bool(false)
===DONE===
