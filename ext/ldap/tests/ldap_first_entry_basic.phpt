--TEST--
ldap_first_entry() - Basic ldap_first_entry test
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
$result = ldap_search($link, "$base", "(objectclass=person)");
var_dump(
	$entry = ldap_first_entry($link, $result),
	ldap_get_values($link, $entry, 'sn')
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
===DONE===
