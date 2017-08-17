--TEST--
ldap_search() test - test that overrides aren't permanent
--CREDITS--
Tyson Andre <tandre@ifwe.co>
# Based on ldap_search_basic.phpt
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifbindfailure.inc');
?>
--FILE--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
ldap_set_option($link, LDAP_OPT_DEREF, LDAP_DEREF_SEARCHING);
ldap_set_option($link, LDAP_OPT_SIZELIMIT, 123);
ldap_set_option($link, LDAP_OPT_TIMELIMIT, 33);
ldap_set_option($link, LDAP_OPT_NETWORK_TIMEOUT, 44);

insert_dummy_data($link, $base);
var_dump(
	$result = ldap_search($link, "$base", "(objectClass=person)", array(), null, 111, 22, LDAP_DEREF_NEVER),
	ldap_get_entries($link, $result)
);
var_dump(
	ldap_get_option($link, LDAP_OPT_DEREF, $option),
	$option,
	ldap_get_option($link, LDAP_OPT_SIZELIMIT, $option),
	$option,
	ldap_get_option($link, LDAP_OPT_TIMELIMIT, $option),
	$option,
	ldap_get_option($link, LDAP_OPT_NETWORK_TIMEOUT, $option),
	$option
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
resource(%d) of type (ldap result)
array(4) {
  ["count"]=>
  int(3)
  [0]=>
  array(14) {
    ["objectclass"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(6) "person"
    }
    [0]=>
    string(11) "objectclass"
    ["cn"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(5) "userA"
    }
    [1]=>
    string(2) "cn"
    ["sn"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(7) "testSN1"
    }
    [2]=>
    string(2) "sn"
    ["userpassword"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(%d) "%s"
    }
    [3]=>
    string(12) "userpassword"
    ["telephonenumber"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(14) "xx-xx-xx-xx-xx"
    }
    [4]=>
    string(15) "telephonenumber"
    ["description"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(6) "user A"
    }
    [5]=>
    string(11) "description"
    ["count"]=>
    int(6)
    ["dn"]=>
    string(%d) "cn=userA,%s"
  }
  [1]=>
  array(12) {
    ["objectclass"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(6) "person"
    }
    [0]=>
    string(11) "objectclass"
    ["cn"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(5) "userB"
    }
    [1]=>
    string(2) "cn"
    ["sn"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(7) "testSN2"
    }
    [2]=>
    string(2) "sn"
    ["userpassword"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(%d) "%s"
    }
    [3]=>
    string(12) "userpassword"
    ["description"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(6) "user B"
    }
    [4]=>
    string(11) "description"
    ["count"]=>
    int(5)
    ["dn"]=>
    string(%d) "cn=userB,%s"
  }
  [2]=>
  array(10) {
    ["objectclass"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(6) "person"
    }
    [0]=>
    string(11) "objectclass"
    ["cn"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(5) "userC"
    }
    [1]=>
    string(2) "cn"
    ["sn"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(7) "testSN3"
    }
    [2]=>
    string(2) "sn"
    ["userpassword"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(%d) "%s"
    }
    [3]=>
    string(12) "userpassword"
    ["count"]=>
    int(4)
    ["dn"]=>
    string(%d) "cn=userC,cn=userB,%s"
  }
}
bool(true)
int(1)
bool(true)
int(123)
bool(true)
int(33)
bool(true)
int(44)
===DONE===
