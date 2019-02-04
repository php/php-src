--TEST--
ldap_ldap_control_paged_result() test (fetching the first page)
--CREDITS--
Jean-Sebastien Hedde <jeanseb@au-fil-du.net>
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifbindfailure.inc');
?>
--FILE--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
insert_dummy_data($link, $base);

$dn = "$base";
$filter = "(cn=user*)";
var_dump(
	ldap_control_paged_result($link, 1),
	$result = ldap_search($link, $dn, $filter, array('cn')),
	ldap_get_entries($link, $result)
);
?>
===DONE===
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
remove_dummy_data($link, $base);
?>
--EXPECTF--
Deprecated: Function ldap_control_paged_result() is deprecated in %s.php on line %d
bool(true)
resource(%d) of type (ldap result)
array(2) {
  ["count"]=>
  int(1)
  [0]=>
  array(4) {
    ["cn"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(5) "userA"
    }
    [0]=>
    string(2) "cn"
    ["count"]=>
    int(1)
    ["dn"]=>
    string(%d) "cn=userA,%s"
  }
}
===DONE===
