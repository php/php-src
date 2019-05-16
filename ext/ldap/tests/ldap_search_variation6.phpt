--TEST--
ldap_search() test
--CREDITS--
Davide Mendolia <idaf1er@gmail.com>
Patrick Allaert <patrickallaert@php.net>
Belgian PHP Testfest 2009
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifbindfailure.inc');
?>
--FILE--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
insert_dummy_data($link, $base);

$dn = "$base";
$filter = "(objectclass=person)";

var_dump(
	$result = ldap_search(array($link, $link), $dn, $filter),
	$result0 = ldap_get_entries($link, $result[0]),
	ldap_get_entries($link, $result[1]) === $result0
);
var_dump(
	$result = ldap_search(array($link, $link), "", $filter),
	ldap_get_entries($link, $result[0]),
	ldap_get_entries($link, $result[1])
);
var_dump(
	$result = ldap_search(array($link, $link), "", array($filter, $filter)),
	ldap_get_entries($link, $result[0]),
	ldap_get_entries($link, $result[1])
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
array(2) {
  [0]=>
  resource(%d) of type (ldap result)
  [1]=>
  resource(%d) of type (ldap result)
}
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
array(2) {
  [0]=>
  resource(%d) of type (ldap result)
  [1]=>
  resource(%d) of type (ldap result)
}
array(1) {
  ["count"]=>
  int(0)
}
array(1) {
  ["count"]=>
  int(0)
}
array(2) {
  [0]=>
  resource(%d) of type (ldap result)
  [1]=>
  resource(%d) of type (ldap result)
}
array(1) {
  ["count"]=>
  int(0)
}
array(1) {
  ["count"]=>
  int(0)
}
===DONE===
