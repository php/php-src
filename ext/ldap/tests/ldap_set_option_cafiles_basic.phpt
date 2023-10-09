--TEST--
ldap_set_option() - Basic test for TLS CA/Cert/CRL/DH/Key file ldap options
--CREDITS--
Chad Sikorra <Chad.Sikorra@gmail.com>
--EXTENSIONS--
ldap
--FILE--
<?php
require "connect.inc";
$link = ldap_connect($uri);

foreach([
    LDAP_OPT_X_TLS_CACERTDIR,
    LDAP_OPT_X_TLS_CACERTFILE,
    LDAP_OPT_X_TLS_CERTFILE,
    LDAP_OPT_X_TLS_KEYFILE,
    LDAP_OPT_X_TLS_CRLFILE,
    LDAP_OPT_X_TLS_DHFILE,
] as $option) {
    $result = ldap_set_option($link, $option, '/foo/bar');
    var_dump($result);

    ldap_get_option($link, $option, $optionval);
    var_dump($optionval);
}
?>
--EXPECT--
bool(true)
string(8) "/foo/bar"
bool(true)
string(8) "/foo/bar"
bool(true)
string(8) "/foo/bar"
bool(true)
string(8) "/foo/bar"
bool(true)
string(8) "/foo/bar"
bool(true)
string(8) "/foo/bar"
