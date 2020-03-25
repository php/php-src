--TEST--
ldap_ldap_control_paged_result() test (fetching the first page then the next final page)
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
$cookie = '';
var_dump(
    ldap_control_paged_result($link, 2, true, $cookie),
    $result = ldap_search($link, $dn, $filter, array('cn')),
    ldap_get_entries($link, $result),
    ldap_control_paged_result_response($link, $result, $cookie),
    ldap_control_paged_result($link, 20, true, $cookie),
    $result = ldap_search($link, $dn, $filter, array('cn')),
    ldap_get_entries($link, $result)
);
?>
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
remove_dummy_data($link, $base);
?>
--EXPECTF--
Deprecated: Function ldap_control_paged_result() is deprecated in %s.php on line %d

Deprecated: Function ldap_control_paged_result_response() is deprecated in %s.php on line %d

Deprecated: Function ldap_control_paged_result() is deprecated in %s.php on line %d
bool(true)
resource(%d) of type (ldap result)
array(3) {
  ["count"]=>
  int(2)
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
  [1]=>
  array(4) {
    ["cn"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(5) "userB"
    }
    [0]=>
    string(2) "cn"
    ["count"]=>
    int(1)
    ["dn"]=>
    string(%d) "cn=userB,%s"
  }
}
bool(true)
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
      string(5) "userC"
    }
    [0]=>
    string(2) "cn"
    ["count"]=>
    int(1)
    ["dn"]=>
    string(%d) "cn=userC,cn=userB,%s"
  }
}
