--TEST--
ldap_mod_replace() - Basic modify operation
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

$entry = array(
	"description" => "user X"
);

var_dump(
	ldap_mod_replace($link, "cn=userA,$base", $entry),
	ldap_get_entries(
		$link,
		ldap_search($link, "$base", "(description=user X)", array("description"))
	)
);
?>
===DONE===
--CLEAN--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);

remove_dummy_data($link, $base);
?>
--EXPECTF--
bool(true)
array(2) {
  ["count"]=>
  int(1)
  [0]=>
  array(4) {
    ["description"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(6) "user X"
    }
    [0]=>
    string(11) "description"
    ["count"]=>
    int(1)
    ["dn"]=>
    string(%d) "cn=userA,%s"
  }
}
===DONE===
