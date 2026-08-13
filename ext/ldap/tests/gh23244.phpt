--TEST--
GH-23244 ldap_set_option() fails for LDAP_OPT_NETWORK_TIMEOUT
--EXTENSIONS--
ldap
--SKIPIF--
<?php
if (!defined('LDAP_OPT_NETWORK_TIMEOUT')) {
    die('skip LDAP_OPT_NETWORK_TIMEOUT is not supported');
}
?>
--FILE--
<?php
$ldap = ldap_connect("ldap://127.0.0.1");

var_dump(ldap_set_option($ldap, LDAP_OPT_NETWORK_TIMEOUT, 3));
var_dump(ldap_get_option($ldap, LDAP_OPT_NETWORK_TIMEOUT, $timeout));
var_dump($timeout);

var_dump(ldap_set_option($ldap, LDAP_OPT_PROTOCOL_VERSION, 3));
var_dump(ldap_get_option($ldap, LDAP_OPT_PROTOCOL_VERSION, $protocol));
var_dump($protocol);
?>
--EXPECT--
bool(true)
bool(true)
int(3)
bool(true)
bool(true)
int(3)
