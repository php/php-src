--TEST--
ldap_first_reference() - Basic ldap_first_reference test
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
insert_dummy_data($link, $base);
ldap_add($link, "cn=userref,$base", array(
        "objectClass" => array("extensibleObject", "referral"),
        "cn" => "userref",
        "ref" => "cn=userA,$base",
));
ldap_set_option($link, LDAP_OPT_DEREF, LDAP_DEREF_NEVER);
$result = ldap_search($link, "$base", "(cn=*)");
var_dump($ref = ldap_first_reference($link, $result));
$refs = null;
ldap_parse_reference($link, $ref, $refs);
var_dump($refs);
?>
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
// Referral can only be removed with Manage DSA IT Control
ldap_delete($link, "cn=userref,$base", [['oid' => LDAP_CONTROL_MANAGEDSAIT, 'iscritical' => TRUE]]);
remove_dummy_data($link, $base);
?>
--EXPECTF--
resource(%d) of type (ldap result entry)
array(1) {
  [0]=>
  string(%d) "cn=userA,%s"
}
