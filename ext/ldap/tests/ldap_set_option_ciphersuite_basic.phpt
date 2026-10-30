--TEST--
ldap_set_option() - Basic test for TLS cipher suite ldap option
--CREDITS--
Chad Sikorra <Chad.Sikorra@gmail.com>
--EXTENSIONS--
ldap
--FILE--
<?php
require "connect.inc";
$link = ldap_connect($uri);

$result = ldap_set_option($link, LDAP_OPT_X_TLS_CIPHER_SUITE, '3DES');
var_dump($result);

ldap_get_option($link, LDAP_OPT_X_TLS_CIPHER_SUITE, $optionval);
var_dump($optionval);
?>
--EXPECT--
bool(true)
string(4) "3DES"
