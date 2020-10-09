--TEST--
ldap_set_option() - Basic test for TCP keepalive ldap options
--CREDITS--
Chad Sikorra <Chad.Sikorra@gmail.com>
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require "connect.inc";
$link = ldap_connect($host, $port);

foreach([
    LDAP_OPT_X_KEEPALIVE_IDLE,
    LDAP_OPT_X_KEEPALIVE_PROBES,
    LDAP_OPT_X_KEEPALIVE_INTERVAL,
] as $option) {
    $result = ldap_set_option($link, $option, 5);
    var_dump($result);

    ldap_get_option($link, $option, $optionval);
    var_dump($optionval);
}
?>
--EXPECT--
bool(true)
int(5)
bool(true)
int(5)
bool(true)
int(5)
