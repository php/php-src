--TEST--
ldap_exop_passwd() - Changing password through EXOP
--CREDITS--
Côme Chilliet <mcmic@php.net>
--EXTENSIONS--
ldap
--SKIPIF--
<?php require_once('skipifbindfailure.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
insert_dummy_data($link, $base);

// ldap_exop_passwd() allows to pass the DN, OLD and NEW passwords,
// and optionally returns the NEW password if none was passed.
var_dump(
  $genpw = ldap_exop_passwd($link, "cn=userA,$base", "oops", "", $ctrls),
  $ctrls,
  $genpw = ldap_exop_passwd($link, "cn=userA,$base"),
  test_bind($host, $port, "cn=userA,$base", $genpw, $protocol_version),
  ldap_exop_passwd($link, "cn=userA,$base", $genpw, "newPassword"),
  test_bind($host, $port, "cn=userA,$base", "newPassword", $protocol_version)
);
?>
--CLEAN--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);

remove_dummy_data($link, $base);
?>
--EXPECTF--
string(%d) "%s"
array(0) {
}
string(%d) "%s"
bool(true)
bool(true)
bool(true)
