--TEST--
ldap_parse_reference() - Basic ldap_parse_reference test
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
ldap_add($link, "cn=userref,dc=my-domain,dc=com", array(
        "objectClass" => array("extensibleObject", "referral"),
        "cn" => "userref",
        "ref" => "cn=userA,dc=my-domain,dc=com",
));
ldap_set_option($link, LDAP_OPT_DEREF, LDAP_DEREF_NEVER);
$result = ldap_search($link, "dc=my-domain,dc=com", "(cn=*)");
$ref = ldap_first_reference($link, $result);
$refs = null;
var_dump(
	ldap_parse_reference($link, $ref, $refs),
	$refs
);
?>
===DONE===
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
// Referral can only be removed with Manage DSA IT Control
ldap_set_option($link, LDAP_OPT_SERVER_CONTROLS, array(array("oid" => "2.16.840.1.113730.3.4.2")));
ldap_delete($link, "cn=userref,dc=my-domain,dc=com");
remove_dummy_data($link);
?>
--EXPECTF--
bool(true)
array(1) {
  [0]=>
  string(28) "cn=userA,dc=my-domain,dc=com"
}
===DONE===
