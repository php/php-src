--TEST--
ldap_result_iteration() - Test iterating over ldap_result
--EXTENSIONS--
ldap
--SKIPIF--
<?php require_once('skipifbindfailure.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);
insert_dummy_data($link, $base);
$result = ldap_list($link, "$base", "(objectClass=person)");
foreach ($result as $dn => $entry) {
    var_dump(
        $dn,
        $entry,
        ldap_get_values($link, $entry, 'sn'),
    );
}
?>
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);
remove_dummy_data($link, $base);
?>
--EXPECTF--
string(%d) "cn=userA,%s"
object(LDAP\ResultEntry)#%d (0) {
}
array(2) {
  [0]=>
  string(7) "testSN1"
  ["count"]=>
  int(1)
}
string(%d) "cn=userB,%s"
object(LDAP\ResultEntry)#%d (0) {
}
array(2) {
  [0]=>
  string(7) "testSN2"
  ["count"]=>
  int(1)
}
