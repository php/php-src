--TEST--
ldap_read() test
--CREDITS--
Davide Mendolia <idaf1er@gmail.com>
Patrick Allaert <patrickallaert@php.net>
Belgian PHP Testfest 2009
--SKIPIF--
<?php 
require_once('skipif.inc'); 
require_once('skipifbindfailure.inc');
?>
--FILE--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
insert_dummy_data($link);
var_dump(
	$result = ldap_read($link, "dc=my-domain,dc=com", "(dc=*)"),
	ldap_get_entries($link, $result)
);
?>
===DONE===
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
remove_dummy_data($link);
?>
--EXPECTF--
resource(%d) of type (ldap result)
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
    string(19) "dc=my-domain,dc=com"
  }
}
===DONE===
