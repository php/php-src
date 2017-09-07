--TEST--
Test the use of controls
--CREDITS--
Côme Chilliet <mcmic@php.net>
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

/* Test assertion control */
var_dump(
	$result = ldap_search($link, "o=test,$base", "objectClass=*", array('o'), 0, 0, 0, LDAP_DEREF_NEVER,
		[['oid' => LDAP_CONTROL_ASSERT, 'value' => ['filter' => '(objectClass=organization)']]]),
	ldap_get_entries($link, $result),
	$result = ldap_search($link, "o=test,$base", "objectClass=*", array('o'), 0, 0, 0, LDAP_DEREF_NEVER,
		[['oid' => LDAP_CONTROL_ASSERT, 'value' => ['filter' => '(objectClass=organizationalUnit)']]])
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
Warning: ldap_search(): Search: Assertion Failed in %s on line %d
resource(%d) of type (ldap result)
array(2) {
  ["count"]=>
  int(1)
  [0]=>
  array(4) {
    ["o"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(4) "test"
    }
    [0]=>
    string(1) "o"
    ["count"]=>
    int(1)
    ["dn"]=>
    string(%d) "o=test,%s"
  }
}
bool(false)
===DONE===
