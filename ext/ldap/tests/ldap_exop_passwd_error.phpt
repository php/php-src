--TEST--
ldap_exop_passwd() - Giving wrong value for old password
--CREDITS--
Côme Chilliet <mcmic@php.net>
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifbindfailure.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
insert_dummy_data($link, $base);

var_dump(ldap_exop_passwd($link, "cn=userA,$base", "wrongPassword", "newPassword"));
var_dump(ldap_error($link));
var_dump(ldap_errno($link));
var_dump(test_bind($host, $port, "cn=userA,$base", "newPassword", $protocol_version));
?>
===DONE===
--CLEAN--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);

remove_dummy_data($link, $base);
?>
--EXPECTF--
Warning: ldap_exop_passwd(): Passwd modify extended operation failed: Server is unwilling to perform (53) in %s on line %d
bool(false)
string(30) "Server is unwilling to perform"
int(53)

Warning: ldap_bind(): Unable to bind to server: Invalid credentials in %s on line %d
bool(false)
===DONE===
