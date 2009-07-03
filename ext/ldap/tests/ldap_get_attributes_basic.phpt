--TEST--
ldap_get_attributes() - Basic ldap_get_attributes test
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
$result = ldap_search($link, "dc=my-domain,dc=com", "(objectclass=organization)");
$entry = ldap_first_entry($link, $result);
var_dump(
	ldap_get_attributes($link, $entry)
);
?>
===DONE===
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
remove_dummy_data($link);
?>
--EXPECT--
array(7) {
  ["objectClass"]=>
  array(4) {
    ["count"]=>
    int(3)
    [0]=>
    string(3) "top"
    [1]=>
    string(8) "dcObject"
    [2]=>
    string(12) "organization"
  }
  [0]=>
  string(11) "objectClass"
  ["dc"]=>
  array(2) {
    ["count"]=>
    int(1)
    [0]=>
    string(9) "my-domain"
  }
  [1]=>
  string(2) "dc"
  ["o"]=>
  array(2) {
    ["count"]=>
    int(1)
    [0]=>
    string(9) "my-domain"
  }
  [2]=>
  string(1) "o"
  ["count"]=>
  int(3)
}
===DONE===
