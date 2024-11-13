--TEST--
ldap_search() test with paged result controls
--CREDITS--
Côme Chilliet <mcmic@php.net>
--EXTENSIONS--
ldap
--SKIPIF--
<?php
require_once('skipifbindfailure.inc');
require_once('skipifcontrol.inc');
skipifunsupportedcontrol(LDAP_CONTROL_PAGEDRESULTS);
?>
--FILE--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);
insert_dummy_data($link, $base);

$dn = "$base";
$filter = "(cn=user*)";
var_dump(
    $result = ldap_search($link, $dn, $filter, array('cn'), 0, 0, 0, LDAP_DEREF_NEVER,
        [['oid' => LDAP_CONTROL_PAGEDRESULTS, 'value' => ['size' => 2]]]),
    ldap_get_entries($link, $result),
    ldap_parse_result($link, $result, $errcode , $matcheddn , $errmsg , $referrals, $controls),
    $result = ldap_search($link, $dn, $filter, array('cn'), 0, 0, 0, LDAP_DEREF_NEVER,
        [['oid' => LDAP_CONTROL_PAGEDRESULTS, 'value' => ['size' => 20, 'cookie' => $controls[LDAP_CONTROL_PAGEDRESULTS]['value']['cookie']]]]),
    ldap_get_entries($link, $result)
);
?>
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);
remove_dummy_data($link, $base);
?>
--EXPECTF--
object(LDAP\Result)#%d (0) {
}
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
object(LDAP\Result)#%d (0) {
}
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
