--TEST--
ldap_get_option() - Basic test for getting the TLS package ldap option
--CREDITS--
Chad Sikorra <Chad.Sikorra@gmail.com>
--EXTENSIONS--
ldap
--FILE--
<?php
require "connect.inc";
$link = ldap_connect($host, $port);

$result = ldap_get_option($link, LDAP_OPT_X_TLS_PACKAGE, $optionval);
var_dump(in_array($optionval, ['GnuTLS', 'OpenSSL', 'MozNSS']));
// This is a read-only option.
var_dump(ldap_set_option($link, LDAP_OPT_X_TLS_PACKAGE, 'foo'));
?>
--EXPECT--
bool(true)
bool(false)
