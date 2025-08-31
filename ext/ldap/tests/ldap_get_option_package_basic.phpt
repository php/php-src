--TEST--
ldap_get_option() - Basic test for getting the TLS package ldap option
--CREDITS--
Chad Sikorra <Chad.Sikorra@gmail.com>
--EXTENSIONS--
ldap
--FILE--
<?php
require "connect.inc";

$result = ldap_get_option(NULL, LDAP_OPT_X_TLS_PACKAGE, $optionval);
var_dump(in_array($optionval, ['GnuTLS', 'OpenSSL', 'MozNSS']));

$link = ldap_connect($uri);

$result = ldap_get_option($link, LDAP_OPT_X_TLS_PACKAGE, $optionval);
var_dump(in_array($optionval, ['GnuTLS', 'OpenSSL', 'MozNSS']));

?>
--EXPECT--
bool(true)
bool(true)
