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
insert_dummy_data($link);

var_dump(
	$result = ldap_search($link, "dc=my-domain,dc=com", "(objectclass=person)", array('sn'), 1),
	ldap_get_entries($link, $result)
);
?>
===DONE===
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
remove_dummy_data($link);
?>
--EXPECTF--
resource(%d) of type (ldap result)
array(4) {
  ["count"]=>
  int(3)
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
    string(28) "cn=userA,dc=my-domain,dc=com"
  }
  [1]=>
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
    string(28) "cn=userB,dc=my-domain,dc=com"
  }
  [2]=>
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
    string(37) "cn=userC,cn=userB,dc=my-domain,dc=com"
  }
}
===DONE===
