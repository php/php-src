--TEST--
ldap_get_attributes() - Basic ldap_get_attributes test
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
$result = ldap_search($link, "$base", "(o=test)");
$entry = ldap_first_entry($link, $result);
var_dump(
    ldap_get_attributes($link, $entry)
);
?>
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
remove_dummy_data($link, $base);
?>
--EXPECT--
array(5) {
  ["objectClass"]=>
  array(3) {
    ["count"]=>
    int(2)
    [0]=>
    string(3) "top"
    [1]=>
    string(12) "organization"
  }
  [0]=>
  string(11) "objectClass"
  ["o"]=>
  array(2) {
    ["count"]=>
    int(1)
    [0]=>
    string(4) "test"
  }
  [1]=>
  string(1) "o"
  ["count"]=>
  int(2)
}
