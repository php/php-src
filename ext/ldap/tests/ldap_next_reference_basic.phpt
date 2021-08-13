--TEST--
ldap_next_reference() - Basic ldap_next_reference test
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--EXTENSIONS--
ldap
--SKIPIF--
<?php require_once('skipifbindfailure.inc'); ?>
--FILE--
<?php
require "connect.inc";
$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
insert_dummy_data($link, $base);
ldap_add($link, "cn=userref,$base", array(
        "objectClass" => array("extensibleObject", "referral"),
        "cn" => "userref",
        "ref" => "cn=userA,$base",
));
ldap_add($link, "cn=userref2,$base", array(
        "objectClass" => array("extensibleObject", "referral"),
        "cn" => "userref2",
        "ref" => "cn=userB,$base",
));
ldap_set_option($link, LDAP_OPT_DEREF, LDAP_DEREF_NEVER);
$result = ldap_search($link, "$base", "(cn=*)");
$ref = ldap_first_reference($link, $result);
var_dump($ref2 = ldap_next_reference($link, $ref));
ldap_parse_reference($link, $ref2, $refs);
var_dump($refs);
?>
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
// Referral can only be removed with Manage DSA IT Control
ldap_delete($link, "cn=userref,$base", [['oid' => LDAP_CONTROL_MANAGEDSAIT, 'iscritical' => TRUE]]);
ldap_delete($link, "cn=userref2,$base", [['oid' => LDAP_CONTROL_MANAGEDSAIT, 'iscritical' => TRUE]]);
remove_dummy_data($link, $base);
?>
--EXPECTF--
object(LDAP\ResultEntry)#%d (0) {
}
array(1) {
  [0]=>
  string(%d) "cn=userB,%s"
}
