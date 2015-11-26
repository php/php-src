--TEST--
ldap_search() bug 48441 - options persists after specifying them in ldap_search
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
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
	$result = ldap_search($link, $dn, $filter, array('sn')),
	ldap_get_entries($link, $result)
);
var_dump(
	$result = ldap_search($link, $dn, $filter, array('sn'), 1, 1, 1, LDAP_DEREF_ALWAYS),
	ldap_get_entries($link, $result)
);
var_dump(
	$result = ldap_search($link, $dn, $filter, array('sn')),
	ldap_get_entries($link, $result)
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
  array(4) {
    ["sn"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(7) "testSN1"
    }
    [0]=>
    string(2) "sn"
    ["count"]=>
    int(1)
    ["dn"]=>
    string(%d) "cn=userA,%s"
  }
  [1]=>
  array(4) {
    ["sn"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(7) "testSN2"
    }
    [0]=>
    string(2) "sn"
    ["count"]=>
    int(1)
    ["dn"]=>
    string(%d) "cn=userB,%s"
  }
  [2]=>
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
    string(%d) "cn=userC,cn=userB,%s"
  }
}

Warning: ldap_search(): Partial search results returned: Sizelimit exceeded in %s on line %d
resource(%d) of type (ldap result)
array(2) {
  ["count"]=>
  int(1)
  [0]=>
  array(4) {
    ["sn"]=>
    array(1) {
      ["count"]=>
      int(0)
    }
    [0]=>
    string(2) "sn"
    ["count"]=>
    int(1)
    ["dn"]=>
    string(%d) "cn=userA,%s"
  }
}
resource(%d) of type (ldap result)
array(4) {
  ["count"]=>
  int(3)
  [0]=>
  array(4) {
    ["sn"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(7) "testSN1"
    }
    [0]=>
    string(2) "sn"
    ["count"]=>
    int(1)
    ["dn"]=>
    string(%d) "cn=userA,%s"
  }
  [1]=>
  array(4) {
    ["sn"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(7) "testSN2"
    }
    [0]=>
    string(2) "sn"
    ["count"]=>
    int(1)
    ["dn"]=>
    string(%d) "cn=userB,%s"
  }
  [2]=>
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
    string(%d) "cn=userC,cn=userB,%s"
  }
}
===DONE===
