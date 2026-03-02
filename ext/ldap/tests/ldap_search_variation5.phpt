--TEST--
ldap_search() test
--CREDITS--
Davide Mendolia <idaf1er@gmail.com>
Patrick Allaert <patrickallaert@php.net>
Belgian PHP Testfest 2009
--EXTENSIONS--
ldap
--SKIPIF--
<?php
require_once('skipifbindfailure.inc');
?>
--FILE--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);
insert_dummy_data($link, $base);

$dn = "$base";
$filter = "(objectclass=person)";
var_dump(
    $result = ldap_search($link, $dn, $filter, array('sn'), 1, 1, 3, LDAP_DEREF_SEARCHING),
    ldap_get_entries($link, $result)
);
var_dump(
    $result = ldap_search($link, $dn, $filter, array('sn'), 1, 1, 3, LDAP_DEREF_FINDING),
    ldap_get_entries($link, $result)
);
var_dump(
    $result = ldap_search($link, $dn, $filter, array('sn'), 1, 1, 3, LDAP_DEREF_ALWAYS),
    ldap_get_entries($link, $result)
);
?>
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);
remove_dummy_data($link, $base);
?>
--EXPECTF--
Warning: ldap_search(): Partial search results returned: Sizelimit exceeded in %s on line %d
object(LDAP\Result)#%d (0) {
}
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

Warning: ldap_search(): Partial search results returned: Sizelimit exceeded in %s on line %d
object(LDAP\Result)#%d (0) {
}
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

Warning: ldap_search(): Partial search results returned: Sizelimit exceeded in %s on line %d
object(LDAP\Result)#%d (0) {
}
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
