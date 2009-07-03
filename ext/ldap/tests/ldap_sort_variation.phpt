--TEST--
ldap_sort() - Basic ldap_sort test
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
insert_dummy_data($link);
ldap_add($link, "cn=userC,dc=my-domain,dc=com", array(
	"objectclass" => "person",
	"cn" => "userC",
	"sn" => "zzz",
	"userPassword" => "oops",
	"description" => "a user",
));
ldap_add($link, "cn=userD,dc=my-domain,dc=com", array(
	"objectclass" => "person",
	"cn" => "userD",
	"sn" => "aaa",
	"userPassword" => "oops",
	"description" => "another user",
));
ldap_add($link, "cn=userE,dc=my-domain,dc=com", array(
	"objectclass" => "person",
	"cn" => "userE",
	"sn" => "a",
	"userPassword" => "oops",
	"description" => "yet another user",
));
$result = ldap_search($link, "dc=my-domain,dc=com", "(objectclass=person)", array("sn", "description"));
var_dump(
	ldap_sort($link, $result, "description"),
	ldap_get_entries($link, $result)
);
?>
===DONE===
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
ldap_delete($link, "cn=userC,dc=my-domain,dc=com");
ldap_delete($link, "cn=userD,dc=my-domain,dc=com");
ldap_delete($link, "cn=userE,dc=my-domain,dc=com");
remove_dummy_data($link);
?>
--EXPECT--
bool(true)
array(7) {
  ["count"]=>
  int(6)
  [0]=>
  array(4) {
    ["sn"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(7) "testSN3"
    }
    [0]=>
    string(2) "sn"
    ["count"]=>
    int(1)
    ["dn"]=>
    string(37) "cn=userC,cn=userB,dc=my-domain,dc=com"
  }
  [1]=>
  array(6) {
    ["sn"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(3) "zzz"
    }
    [0]=>
    string(2) "sn"
    ["description"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(6) "a user"
    }
    [1]=>
    string(11) "description"
    ["count"]=>
    int(2)
    ["dn"]=>
    string(28) "cn=userC,dc=my-domain,dc=com"
  }
  [2]=>
  array(6) {
    ["sn"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(3) "aaa"
    }
    [0]=>
    string(2) "sn"
    ["description"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(12) "another user"
    }
    [1]=>
    string(11) "description"
    ["count"]=>
    int(2)
    ["dn"]=>
    string(28) "cn=userD,dc=my-domain,dc=com"
  }
  [3]=>
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
    ["description"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(6) "user A"
    }
    [1]=>
    string(11) "description"
    ["count"]=>
    int(2)
    ["dn"]=>
    string(28) "cn=userA,dc=my-domain,dc=com"
  }
  [4]=>
  array(6) {
    ["sn"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(7) "testSN2"
    }
    [0]=>
    string(2) "sn"
    ["description"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(6) "user B"
    }
    [1]=>
    string(11) "description"
    ["count"]=>
    int(2)
    ["dn"]=>
    string(28) "cn=userB,dc=my-domain,dc=com"
  }
  [5]=>
  array(6) {
    ["sn"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(1) "a"
    }
    [0]=>
    string(2) "sn"
    ["description"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(16) "yet another user"
    }
    [1]=>
    string(11) "description"
    ["count"]=>
    int(2)
    ["dn"]=>
    string(28) "cn=userE,dc=my-domain,dc=com"
  }
}
===DONE===
