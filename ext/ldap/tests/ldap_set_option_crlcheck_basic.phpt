--TEST--
ldap_set_option() - Basic test for TLS CRL check ldap option
--CREDITS--
Chad Sikorra <Chad.Sikorra@gmail.com>
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php
    require "connect.inc";
    $link = ldap_connect($host, $port);
    ldap_get_option($link, LDAP_OPT_X_TLS_PACKAGE, $package);
    if ($package != 'OpenSSL') {
        die("skip OpenSSL required for CRL check options, got: $package");
    }
?>
--FILE--
<?php
require "connect.inc";
$link = ldap_connect($host, $port);

foreach([
    LDAP_OPT_X_TLS_CRL_NONE,
    LDAP_OPT_X_TLS_CRL_PEER,
    LDAP_OPT_X_TLS_CRL_ALL,
] as $option) {
    $result = ldap_set_option($link, LDAP_OPT_X_TLS_CRLCHECK, $option);
    var_dump($result);

    ldap_get_option($link, LDAP_OPT_X_TLS_CRLCHECK, $optionval);
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
