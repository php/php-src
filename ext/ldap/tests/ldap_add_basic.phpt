--TEST--
ldap_add() - Basic add operation
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

var_dump(
    ldap_add($link, "dc=my-domain,$base", array(
        "objectClass"	=> array(
            "top",
            "dcObject",
            "organization"),
        "dc"			=> "my-domain",
        "o"				=> "my-domain",
    )),
    ldap_get_entries(
        $link,
        ldap_search($link, "$base", "(o=my-domain)")
    )
);
?>
--CLEAN--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);

ldap_delete($link, "dc=my-domain,$base");
?>
--EXPECTF--
bool(true)
array(2) {
  ["count"]=>
  int(1)
  [0]=>
  array(8) {
    ["objectclass"]=>
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
    string(11) "objectclass"
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
    ["dn"]=>
    string(%d) "dc=my-domain,%s"
  }
}
