--TEST--
ldap_modify() - Basic modify operation
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

$entry = array(
	"objectClass"	=> array(
		"top",
		"dcObject",
		"organization"),
	"dc"		=> "my-domain",
	"o"		=> "my-domain",
	"description"	=> "Domain description",
);

var_dump(
	ldap_modify($link, "dc=my-domain,dc=com", $entry),
	ldap_get_entries(
		$link,
		ldap_search($link, "dc=my-domain,dc=com", "(Description=Domain description)")
	)
);
?>
===DONE===
--CLEAN--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);

remove_dummy_data($link);
?>
--EXPECT--
bool(true)
array(2) {
  ["count"]=>
  int(1)
  [0]=>
  array(10) {
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
    ["description"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(18) "Domain description"
    }
    [3]=>
    string(11) "description"
    ["count"]=>
    int(4)
    ["dn"]=>
    string(19) "dc=my-domain,dc=com"
  }
}
===DONE===
