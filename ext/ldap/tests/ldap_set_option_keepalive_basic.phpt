--TEST--
ldap_set_option() - Basic test for TCP keepalive ldap options
--CREDITS--
Chad Sikorra <Chad.Sikorra@gmail.com>
--EXTENSIONS--
ldap
--FILE--
<?php
require "connect.inc";
$link = ldap_connect($uri);

foreach([
    LDAP_OPT_X_KEEPALIVE_IDLE,
    LDAP_OPT_X_KEEPALIVE_PROBES,
    LDAP_OPT_X_KEEPALIVE_INTERVAL,
] as $option) {
    var_dump(
        ldap_set_option($link, $option, 5),
        ldap_get_option($link, $option, $optionval),
        $optionval,
        $link->setOption($option, 6),
        $link->getOption($option),
    );
}
?>
--EXPECT--
bool(true)
bool(true)
int(5)
NULL
int(6)
bool(true)
bool(true)
int(5)
NULL
int(6)
bool(true)
bool(true)
int(5)
NULL
int(6)
