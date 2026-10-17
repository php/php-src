--TEST--
ldap_exop_sync() - without the response_data argument
--EXTENSIONS--
ldap
--SKIPIF--
<?php require_once('skipifbindfailure.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);

var_dump(
    ldap_exop_sync($link, LDAP_EXOP_WHO_AM_I),
    ldap_exop_sync($link, LDAP_EXOP_WHO_AM_I, null, null, $retdata),
    $retdata
);
?>
--EXPECTF--
bool(true)
bool(true)
string(%d) "dn:%s"
