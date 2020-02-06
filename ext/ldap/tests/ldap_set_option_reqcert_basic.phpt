--TEST--
ldap_option_reqcert_basic() - Error test for TLS require cert ldap option
--SKIPIF--
<?php require 'skipif.inc'; ?>
--CREDITS--
Edwin Hoksberg <edwin@edwinhoksberg.nl>
--FILE--
<?php
require "connect.inc";
$link = ldap_connect($host, $port);

foreach([
    LDAP_OPT_X_TLS_NEVER,
    LDAP_OPT_X_TLS_HARD,
    LDAP_OPT_X_TLS_DEMAND,
    LDAP_OPT_X_TLS_ALLOW,
    LDAP_OPT_X_TLS_TRY,
] as $option) {
    $result = ldap_set_option($link, LDAP_OPT_X_TLS_REQUIRE_CERT, $option);
    var_dump($result);

    ldap_get_option($link, LDAP_OPT_X_TLS_REQUIRE_CERT, $optionval);
    var_dump($optionval);
}
?>
--EXPECT--
bool(true)
int(0)
bool(true)
int(1)
bool(true)
int(2)
bool(true)
int(3)
bool(true)
int(4)
