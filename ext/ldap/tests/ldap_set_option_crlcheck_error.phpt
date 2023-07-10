--TEST--
ldap_set_option() - Error test for TLS CRL check ldap option
--CREDITS--
Chad Sikorra <Chad.Sikorra@gmail.com>
--EXTENSIONS--
ldap
--FILE--
<?php
require "connect.inc";
$link = ldap_connect($uri);
$result = ldap_set_option($link, LDAP_OPT_X_TLS_CRLCHECK, 9001);
var_dump($result);
?>
--EXPECT--
bool(false)
