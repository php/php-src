--TEST--
ldap_add_ext() - Add operation with controls
--CREDITS--
Côme Chilliet <mcmic@php.net>
--EXTENSIONS--
ldap
--SKIPIF--
<?php require_once('skipifbindfailure.inc'); ?>
<?php
require_once('skipifcontrol.inc');
skipifunsupportedcontrol(LDAP_CONTROL_POST_READ);
?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);

var_dump(
    $result = ldap_add_ext($link, "o=test_ldap_add_ext,$base", array(
        "objectClass"	=> array(
            "top",
            "organization"),
        "o"	=> "test_ldap_add_ext",
    ), [['oid' => LDAP_CONTROL_POST_READ, 'iscritical' => TRUE, 'value' => ['attrs' => ['o']]]]),
    ldap_parse_result($link, $result, $errcode, $matcheddn, $errmsg, $referrals, $ctrls),
    $errcode,
    $errmsg,
    $ctrls[LDAP_CONTROL_POST_READ],
    ldap_get_entries(
        $link,
        ldap_search($link, "$base", "(o=test_ldap_add_ext)")
    )
);
?>
--CLEAN--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);

ldap_delete($link, "o=test_ldap_add_ext,$base");
?>
--EXPECTF--
object(LDAP\Result)#%d (0) {
}
bool(true)
int(0)
string(0) ""
array(2) {
  ["oid"]=>
  string(14) "1.3.6.1.1.13.2"
  ["value"]=>
  array(2) {
    ["dn"]=>
    string(%d) "o=test_ldap_add_ext,%s"
    ["o"]=>
    array(1) {
      [0]=>
      string(17) "test_ldap_add_ext"
    }
  }
}
array(2) {
  ["count"]=>
  int(1)
  [0]=>
  array(6) {
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
      string(17) "test_ldap_add_ext"
    }
    [1]=>
    string(1) "o"
    ["count"]=>
    int(2)
    ["dn"]=>
    string(%d) "o=test_ldap_add_ext,%s"
  }
}
