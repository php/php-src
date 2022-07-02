--TEST--
ldap_next_attribute() - Basic ldap_next_attribute test
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
$result = ldap_search($link, "$base", "(cn=userC)");
$entry = ldap_first_entry($link, $result);
$attribute = ldap_first_attribute($link, $entry);
var_dump(
    ldap_next_attribute($link, $entry),
    ldap_next_attribute($link, $entry),
    ldap_next_attribute($link, $entry),
    ldap_next_attribute($link, $entry)
);
?>
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
remove_dummy_data($link, $base);
?>
--EXPECTF--
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
bool(false)
