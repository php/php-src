--TEST--
ldap_search() test
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

$dn = "dc=my-domain,dc=com";
$filter = "(dc=*)";
var_dump(
	$result = ldap_search($link, "dc=my-domain,dc=com", "(dc=*)", array('dc')),
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
  array(4) {
    ["dc"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(9) "my-domain"
    }
    [0]=>
    string(2) "dc"
    ["count"]=>
    int(1)
    ["dn"]=>
    string(19) "dc=my-domain,dc=com"
  }
}
===DONE===
