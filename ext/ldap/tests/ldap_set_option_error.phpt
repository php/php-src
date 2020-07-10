--TEST--
ldap_set_option() - ldap_set_option() operation that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect($host, $port);
$controls = array(
    array(
        array("xid" => "1.2.752.58.10.1", "iscritical" => true),
        array("xid" => "1.2.752.58.1.10", "value" => "magic"),
    ),
    array(
        array("oid" => "1.2.752.58.10.1", "iscritical" => true),
        array("oid" => "1.2.752.58.1.10", "value" => "magic"),
        "weird"
    ),
    "notanarray"
);

var_dump(ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, 10));

foreach ($controls as $control)
    var_dump(ldap_set_option($link, LDAP_OPT_SERVER_CONTROLS, $control));

var_dump(ldap_set_option($link, 999999, 999999));
?>
--EXPECTF--
bool(false)

Warning: ldap_set_option(): Control must have an "oid" key in %s on line %d
bool(false)

Warning: ldap_set_option(): The array value must contain only arrays, where each array is a control in %s on line %d
bool(false)

Warning: ldap_set_option(): Expected array value for this option in %s on line %d
bool(false)
bool(false)
