--TEST--
ldap_search() test with sort and VLV controls
--CREDITS--
Côme Chilliet <mcmic@php.net>
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifbindfailure.inc');
require_once('skipifcontrol.inc');
skipifunsupportedcontrol(LDAP_CONTROL_SORTREQUEST);
skipifunsupportedcontrol(LDAP_CONTROL_VLVREQUEST);
?>
--FILE--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
insert_dummy_data($link, $base);

/* First test with only SORT control */
var_dump(
    $result = ldap_search($link, $base, '(cn=*)', array('cn'), 0, 0, 0, LDAP_DEREF_NEVER,
        [
            [
                'oid' => LDAP_CONTROL_SORTREQUEST,
                'iscritical' => TRUE,
                'value' => [
                    ['attr' => 'cn', 'oid' => '2.5.13.3' /* caseIgnoreOrderingMatch */, 'reverse' => TRUE]
                ]
            ]
        ]
    ),
    ldap_get_entries($link, $result),
    ldap_parse_result($link, $result, $errcode , $matcheddn , $errmsg , $referrals, $controls),
    $errcode,
    $errmsg,
    $controls
);

/* Then with VLV control */
var_dump(
    $result = ldap_search($link, $base, '(cn=*)', array('cn'), 0, 0, 0, LDAP_DEREF_NEVER,
        [
            [
                'oid' => LDAP_CONTROL_SORTREQUEST,
                'iscritical' => TRUE,
                'value' => [
                    ['attr' => 'cn', 'oid' => '2.5.13.3' /* caseIgnoreOrderingMatch */, 'reverse' => TRUE]
                ]
            ],
            [
                'oid' => LDAP_CONTROL_VLVREQUEST,
                'iscritical' => TRUE,
                'value' => [
                    'before'	=> 0, // Return 0 entry before target
                    'after'		=> 1, // Return 1 entry after target
                    'offset'	=> 2, // Target entry is the second one
                    'count'		=> 0, // We have no idea how many entries there are
                ]
            ]
        ]
    ),
    ldap_get_entries($link, $result),
    ldap_parse_result($link, $result, $errcode , $matcheddn , $errmsg , $referrals, $controls),
    array_keys($controls),
    $controls[LDAP_CONTROL_SORTRESPONSE],
    $controls[LDAP_CONTROL_VLVRESPONSE]['value']['target'],
    $controls[LDAP_CONTROL_VLVRESPONSE]['value']['count'],
    $controls[LDAP_CONTROL_VLVRESPONSE]['value']['errcode'],
    bin2hex($controls[LDAP_CONTROL_VLVRESPONSE]['value']['context'])
);
?>
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
remove_dummy_data($link, $base);
?>
--EXPECTF--
resource(%d) of type (ldap result)
array(4) {
  ["count"]=>
  int(3)
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
  [2]=>
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
bool(true)
int(0)
string(0) ""
array(1) {
  ["1.2.840.113556.1.4.474"]=>
  array(2) {
    ["oid"]=>
    string(22) "1.2.840.113556.1.4.474"
    ["value"]=>
    array(1) {
      ["errcode"]=>
      int(0)
    }
  }
}
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
      string(5) "userB"
    }
    [0]=>
    string(2) "cn"
    ["count"]=>
    int(1)
    ["dn"]=>
    string(%d) "cn=userB,%s"
  }
  [1]=>
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
bool(true)
array(2) {
  [0]=>
  string(22) "1.2.840.113556.1.4.474"
  [1]=>
  string(24) "2.16.840.1.113730.3.4.10"
}
array(2) {
  ["oid"]=>
  string(22) "1.2.840.113556.1.4.474"
  ["value"]=>
  array(1) {
    ["errcode"]=>
    int(0)
  }
}
int(2)
int(3)
int(0)
string(%d) "%s"
