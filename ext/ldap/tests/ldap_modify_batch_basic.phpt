--TEST--
ldap_modify_batch() - Basic batch modify operation
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
Ondřej Hošek <ondra.hosek@gmail.com>
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifbindfailure.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
insert_dummy_data($link);

$mods = array(
	array(
		"attrib"	=> "telephoneNumber",
		"modtype"	=> LDAP_MODIFY_BATCH_ADD,
		"values"	=> array(
			"+1 555 5551717"
		)
	),
	array(
		"attrib"	=> "sn",
		"modtype"	=> LDAP_MODIFY_BATCH_REPLACE,
		"values"	=> array("Brown-Smith")
	),
	array(
		"attrib"	=> "description",
		"modtype"	=> LDAP_MODIFY_BATCH_REMOVE_ALL
	)
);

var_dump(
	ldap_modify_batch($link, "cn=userA,dc=my-domain,dc=com", $mods),
	ldap_get_entries($link, ldap_search($link, "dc=my-domain,dc=com", "(sn=Brown-Smith)"))
);
?>
===DONE===
--CLEAN--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);

remove_dummy_data($link);
?>
--EXPECT--
bool(true)
array(2) {
  ["count"]=>
  int(1)
  [0]=>
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
      string(5) "userA"
    }
    [1]=>
    string(2) "cn"
    ["userpassword"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(4) "oops"
    }
    [2]=>
    string(12) "userpassword"
    ["telephonenumber"]=>
    array(3) {
      ["count"]=>
      int(2)
      [0]=>
      string(14) "xx-xx-xx-xx-xx"
      [1]=>
      string(14) "+1 555 5551717"
    }
    [3]=>
    string(15) "telephonenumber"
    ["sn"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(11) "Brown-Smith"
    }
    [4]=>
    string(2) "sn"
    ["count"]=>
    int(5)
    ["dn"]=>
    string(28) "cn=userA,dc=my-domain,dc=com"
  }
}
===DONE===
