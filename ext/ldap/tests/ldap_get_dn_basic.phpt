--TEST--
ldap_get_dn() - Basic ldap_get_dn test
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
insert_dummy_data($link, $base);
$result = ldap_search($link, "$base", "(objectclass=organization)");
$entry = ldap_first_entry($link, $result);
var_dump(
    ldap_get_dn($link, $entry)
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
