--TEST--
ldap_get_values_len() - Testing ldap_get_values_len() that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--EXTENSIONS--
ldap
--SKIPIF--
<?php require_once('skipifbindfailure.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
insert_dummy_data($link, $base);
$result = ldap_search($link, "$base", "(objectclass=organization)");
$entry = ldap_first_entry($link, $result);

var_dump(ldap_get_values_len($link, $entry, "inexistentAttribute"));
?>
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
remove_dummy_data($link, $base);
?>
--EXPECTF--
Warning: ldap_get_values_len(): Cannot get the value(s) of attribute %s in %s on line %d
bool(false)
