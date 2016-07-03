--TEST--
ldap_rename() - Basic ldap_rename test
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
	ldap_rename($link, "cn=userA,$base", "cn=userZ", "$base", true)
);
$result = ldap_search($link, "$base", "(cn=userA)", array("cn", "sn"));
$result = ldap_search($link, "$base", "(cn=userZ)", array("cn", "sn"));
var_dump(ldap_get_entries($link, $result));
?>
===DONE===
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
ldap_rename($link, "cn=userZ,$base", "cn=userA", "$base", true);
remove_dummy_data($link, $base);
?>
--EXPECTF--
bool(true)
array(2) {
  ["count"]=>
  int(1)
  [0]=>
  array(6) {
    ["sn"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(7) "testSN1"
    }
    [0]=>
    string(2) "sn"
    ["cn"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(5) "userZ"
    }
    [1]=>
    string(2) "cn"
    ["count"]=>
    int(2)
    ["dn"]=>
    string(%d) "cn=userZ,%s"
  }
}
===DONE===
