--TEST--
ldap_set_option() - Basic test for TLS protocol max ldap option
--CREDITS--
Chad Sikorra <Chad.Sikorra@gmail.com>
--EXTENSIONS--
ldap
--FILE--
<?php
require "connect.inc";
$link = ldap_connect($uri);

foreach([
    LDAP_OPT_X_TLS_PROTOCOL_SSL2,
    LDAP_OPT_X_TLS_PROTOCOL_SSL3,
    LDAP_OPT_X_TLS_PROTOCOL_TLS1_0,
    LDAP_OPT_X_TLS_PROTOCOL_TLS1_1,
    LDAP_OPT_X_TLS_PROTOCOL_TLS1_2,
] as $option) {
    $result = ldap_set_option($link, LDAP_OPT_X_TLS_PROTOCOL_MAX, $option);
    var_dump($result);

    ldap_get_option($link, LDAP_OPT_X_TLS_PROTOCOL_MAX, $optionval);
    var_dump($optionval);
}
?>
--EXPECT--
bool(true)
int(512)
bool(true)
int(768)
bool(true)
int(769)
bool(true)
int(770)
bool(true)
int(771)
