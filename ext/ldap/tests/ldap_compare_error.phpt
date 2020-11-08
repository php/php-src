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
insert_dummy_data($link, $base);

var_dump(
    ldap_compare($link, "cn=userNotAvailable,$base", "sn", "testSN1"),
    ldap_error($link),
    ldap_errno($link)
);
?>
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
remove_dummy_data($link, $base);
?>
--EXPECTF--
Warning: ldap_compare(): Compare: No such object in %s on line %d
int(-1)
string(14) "No such object"
int(32)
