--TEST--
ldap_delete_ext() - Delete operation with controls
--CREDITS--
Côme Chilliet <mcmic@php.net>
--EXTENSIONS--
ldap
--SKIPIF--
<?php require_once('skipifbindfailure.inc'); ?>
<?php
require_once('skipifcontrol.inc');
skipifunsupportedcontrol(LDAP_CONTROL_PRE_READ);
?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
ldap_add($link, "dc=my-domain,$base", array(
    "objectClass"	=> array(
        "top",
        "dcObject",
        "organization"),
    "dc"			=> "my-domain",
    "o"				=> "my-domain",
));

var_dump(
    $result = ldap_delete_ext($link, "dc=my-domain,$base",
        [['oid' => LDAP_CONTROL_PRE_READ, 'iscritical' => TRUE, 'value' => ['attrs' => ['dc', 'o']]]]
    ),
    ldap_parse_result($link, $result, $errcode, $matcheddn, $errmsg, $referrals, $ctrls),
    $errcode,
    $errmsg,
    $ctrls[LDAP_CONTROL_PRE_READ],
    @ldap_search($link, "dc=my-domain,$base", "(o=my-domain)")
);
?>
--CLEAN--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);

@ldap_delete($link, "dc=my-domain,$base");
?>
--EXPECTF--
object(LDAP\Result)#%d (0) {
}
bool(true)
int(0)
string(0) ""
array(2) {
  ["oid"]=>
  string(14) "1.3.6.1.1.13.1"
  ["value"]=>
  array(3) {
    ["dn"]=>
    string(%d) "dc=my-domain,%s"
    ["dc"]=>
    array(1) {
      [0]=>
      string(9) "my-domain"
    }
    ["o"]=>
    array(1) {
      [0]=>
      string(9) "my-domain"
    }
  }
}
bool(false)
