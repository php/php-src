--TEST--
ldap_exop_whoami() - EXOP whoami operation
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

// ldap_exop_whoami(resource link [, string authzid])
var_dump(
  ldap_exop_whoami($link, $authzid),
  $authzid,
  $r = ldap_exop_whoami($link),
  ldap_parse_exop_whoami($link, $r, $result),
  $result
);
?>
===DONE===
--CLEAN--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);

remove_dummy_data($link, $base);
?>
--EXPECTF--
bool(true)
string(%d) "dn:%s"
resource(%d) of type (ldap result)
bool(true)
string(%d) "dn:%s"
===DONE===
