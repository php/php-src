--TEST--
ldap_rename_ext() - Rename operation with controls
--CREDITS--
Côme Chilliet <mcmic@php.net>
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifbindfailure.inc'); ?>
<?php
require_once('skipifcontrol.inc');
skipifunsupportedcontrol(LDAP_CONTROL_PRE_READ);
skipifunsupportedcontrol(LDAP_CONTROL_POST_READ);
?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
insert_dummy_data($link, $base);

var_dump(
    $result = ldap_rename_ext($link, "cn=userA,$base", "cn=userZ", "$base", TRUE,
        [
            ['oid' => LDAP_CONTROL_PRE_READ,  'iscritical' => TRUE, 'value' => ['attrs' => ['cn']]],
            ['oid' => LDAP_CONTROL_POST_READ, 'iscritical' => TRUE, 'value' => ['attrs' => ['cn']]]
        ]
    ),
    ldap_parse_result($link, $result, $errcode, $matcheddn, $errmsg, $referrals, $ctrls),
    $errcode,
    $errmsg,
    $ctrls[LDAP_CONTROL_PRE_READ],
    $ctrls[LDAP_CONTROL_POST_READ],
    ldap_count_entries($link, ldap_search($link, "$base", "(cn=userA)", array("cn"))),
    ldap_count_entries($link, ldap_search($link, "$base", "(cn=userZ)", array("cn")))
);
?>
--CLEAN--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
ldap_rename($link, "cn=userZ,$base", "cn=userA", "$base", true);
remove_dummy_data($link, $base);
?>
--EXPECTF--
resource(%d) of type (ldap result)
bool(true)
int(0)
string(0) ""
array(2) {
  ["oid"]=>
  string(14) "1.3.6.1.1.13.1"
  ["value"]=>
  array(2) {
    ["dn"]=>
    string(%d) "cn=userA,%s"
    ["cn"]=>
    array(1) {
      [0]=>
      string(5) "userA"
    }
  }
}
array(2) {
  ["oid"]=>
  string(14) "1.3.6.1.1.13.2"
  ["value"]=>
  array(2) {
    ["dn"]=>
    string(%d) "cn=userZ,%s"
    ["cn"]=>
    array(1) {
      [0]=>
      string(5) "userZ"
    }
  }
}
int(0)
int(1)
