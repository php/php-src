--TEST--
ldap_mod_add() - Basic modify operation
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

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);
insert_dummy_data($link, $base);

$entry = array(
    "description"	=> "Domain description",
);

var_dump(
    ldap_mod_add($link, "o=test,$base", $entry),
    ldap_get_entries(
        $link,
        ldap_search($link, "o=test,$base", "(Description=Domain description)")
    )
);
?>
--CLEAN--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);

remove_dummy_data($link, $base);
?>
--EXPECTF--
bool(true)
array(2) {
  ["count"]=>
  int(1)
  [0]=>
  array(8) {
    ["objectclass"]=>
    array(3) {
      ["count"]=>
      int(2)
      [0]=>
      string(3) "top"
      [1]=>
      string(12) "organization"
    }
    [0]=>
    string(11) "objectclass"
    ["o"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(4) "test"
    }
    [1]=>
    string(1) "o"
    ["description"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(18) "Domain description"
    }
    [2]=>
    string(11) "description"
    ["count"]=>
    int(3)
    ["dn"]=>
    string(%d) "o=test,%s"
  }
}
