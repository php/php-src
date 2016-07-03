--TEST--
ldap_next_entry() - Basic ldap_first_entry test
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
$result = ldap_list($link, "$base", "(objectClass=person)");
$entry = ldap_first_entry($link, $result);
var_dump(
	$entry = ldap_next_entry($link, $entry),
	ldap_get_values($link, $entry, 'sn'),
	$entry = ldap_next_entry($link, $entry)
);
?>
===DONE===
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
remove_dummy_data($link, $base);
?>
--EXPECTF--
resource(%d) of type (ldap result entry)
array(2) {
  [0]=>
  string(7) "testSN%d"
  ["count"]=>
  int(1)
}
bool(false)
===DONE===
