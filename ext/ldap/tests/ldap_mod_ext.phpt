--TEST--
ldap_mod_ext() - Modify operations with controls
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--EXTENSIONS--
ldap
--SKIPIF--
<?php require_once('skipifbindfailure.inc'); ?>
<?php
require_once('skipifcontrol.inc');
skipifunsupportedcontrol(LDAP_CONTROL_PRE_READ);
skipifunsupportedcontrol(LDAP_CONTROL_POST_READ);
?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);
insert_dummy_data($link, $base);

$entry = array(
    "description"	=> "Domain description",
);

var_dump(
    $result = ldap_mod_add_ext($link, "o=test,$base", $entry,
        [
            ['oid' => LDAP_CONTROL_PRE_READ,  'iscritical' => TRUE, 'value' => ['attrs' => ['description']]],
            ['oid' => LDAP_CONTROL_POST_READ, 'iscritical' => TRUE, 'value' => ['attrs' => ['description']]],
        ]
    ),
    ldap_parse_result($link, $result, $errcode, $matcheddn, $errmsg, $referrals, $ctrls),
    $errcode,
    $errmsg,
    $ctrls,
    ldap_get_entries(
        $link,
        ldap_search($link, "o=test,$base", "(Description=Domain description)")
    ),
    $result = ldap_mod_del_ext($link, "o=test,$base", $entry,
        [
            ['oid' => LDAP_CONTROL_PRE_READ,  'iscritical' => TRUE, 'value' => ['attrs' => ['description']]],
            ['oid' => LDAP_CONTROL_POST_READ, 'iscritical' => TRUE, 'value' => ['attrs' => ['description']]],
        ]
    ),
    ldap_parse_result($link, $result, $errcode, $matcheddn, $errmsg, $referrals, $ctrls),
    $errcode,
    $errmsg,
    $ctrls,
    ldap_get_entries(
        $link,
        ldap_search($link, "o=test,$base", "(Description=Domain description)")
    )
);
?>
--CLEAN--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);

remove_dummy_data($link, $base);
?>
--EXPECTF--
object(LDAP\Result)#%d (0) {
}
bool(true)
int(0)
string(0) ""
array(2) {
  ["1.3.6.1.1.13.1"]=>
  array(2) {
    ["oid"]=>
    string(14) "1.3.6.1.1.13.1"
    ["value"]=>
    array(1) {
      ["dn"]=>
      string(%d) "o=test,%s"
    }
  }
  ["1.3.6.1.1.13.2"]=>
  array(2) {
    ["oid"]=>
    string(14) "1.3.6.1.1.13.2"
    ["value"]=>
    array(2) {
      ["dn"]=>
      string(%d) "o=test,%s"
      ["description"]=>
      array(1) {
        [0]=>
        string(18) "Domain description"
      }
    }
  }
}
array(2) {
  ["count"]=>
  int(1)
  [0]=>
  array(8) {
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
      string(4) "test"
    }
    [1]=>
    string(1) "o"
    ["description"]=>
    array(2) {
      ["count"]=>
      int(1)
      [0]=>
      string(18) "Domain description"
    }
    [2]=>
    string(11) "description"
    ["count"]=>
    int(3)
    ["dn"]=>
    string(%d) "o=test,%s"
  }
}
object(LDAP\Result)#%d (0) {
}
bool(true)
int(0)
string(0) ""
array(2) {
  ["1.3.6.1.1.13.1"]=>
  array(2) {
    ["oid"]=>
    string(14) "1.3.6.1.1.13.1"
    ["value"]=>
    array(2) {
      ["dn"]=>
      string(%d) "o=test,%s"
      ["description"]=>
      array(1) {
        [0]=>
        string(18) "Domain description"
      }
    }
  }
  ["1.3.6.1.1.13.2"]=>
  array(2) {
    ["oid"]=>
    string(14) "1.3.6.1.1.13.2"
    ["value"]=>
    array(1) {
      ["dn"]=>
      string(%d) "o=test,%s"
    }
  }
}
array(1) {
  ["count"]=>
  int(0)
}
