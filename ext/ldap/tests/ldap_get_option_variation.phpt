--TEST--
ldap_get_option() - More ldap_get_option() operations
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect($host, $port);
$option = null;

$controls = array(
    array("oid" => "1.2.752.58.10.1", "iscritical" => true),
    array("oid" => "1.2.752.58.1.10", "value" => "magic"),
);

ldap_set_option($link, LDAP_OPT_DEREF, LDAP_DEREF_NEVER);
ldap_set_option($link, LDAP_OPT_SIZELIMIT, 123);
ldap_set_option($link, LDAP_OPT_TIMELIMIT, 33);
ldap_set_option($link, LDAP_OPT_NETWORK_TIMEOUT, 44);
ldap_set_option($link, LDAP_OPT_TIMEOUT, 55);
ldap_set_option($link, LDAP_OPT_REFERRALS, false);
ldap_set_option($link, LDAP_OPT_SERVER_CONTROLS, $controls);
ldap_set_option($link, LDAP_OPT_CLIENT_CONTROLS, $controls);
ldap_set_option($link, LDAP_OPT_RESTART, false);

var_dump(
    ldap_get_option($link, LDAP_OPT_DEREF, $option),
    $option,
    ldap_get_option($link, LDAP_OPT_SIZELIMIT, $option),
    $option,
    ldap_get_option($link, LDAP_OPT_TIMELIMIT, $option),
    $option,
    ldap_get_option($link, LDAP_OPT_NETWORK_TIMEOUT, $option),
    $option,
    ldap_get_option($link, LDAP_OPT_TIMEOUT, $option),
    $option,
    ldap_get_option($link, LDAP_OPT_REFERRALS, $option),
    $option,
    ldap_get_option($link, LDAP_OPT_RESTART, $option),
    $option,
    ldap_get_option($link, LDAP_OPT_SERVER_CONTROLS, $option),
    $option,
    ldap_get_option($link, LDAP_OPT_CLIENT_CONTROLS, $option),
    $option
);
?>
--EXPECT--
bool(true)
int(0)
bool(true)
int(123)
bool(true)
int(33)
bool(true)
int(44)
bool(true)
int(55)
bool(true)
int(0)
bool(true)
int(0)
bool(true)
array(2) {
  ["1.2.752.58.10.1"]=>
  array(3) {
    ["oid"]=>
    string(15) "1.2.752.58.10.1"
    ["iscritical"]=>
    bool(true)
    ["value"]=>
    NULL
  }
  ["1.2.752.58.1.10"]=>
  array(3) {
    ["oid"]=>
    string(15) "1.2.752.58.1.10"
    ["iscritical"]=>
    bool(false)
    ["value"]=>
    string(5) "magic"
  }
}
bool(true)
array(2) {
  ["1.2.752.58.10.1"]=>
  array(3) {
    ["oid"]=>
    string(15) "1.2.752.58.10.1"
    ["iscritical"]=>
    bool(true)
    ["value"]=>
    NULL
  }
  ["1.2.752.58.1.10"]=>
  array(3) {
    ["oid"]=>
    string(15) "1.2.752.58.1.10"
    ["iscritical"]=>
    bool(false)
    ["value"]=>
    string(5) "magic"
  }
}
