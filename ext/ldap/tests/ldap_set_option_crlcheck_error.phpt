--TEST--
ldap_set_option() - Error test for TLS CRL check ldap option
--CREDITS--
Chad Sikorra <Chad.Sikorra@gmail.com>
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require "connect.inc";
$link = ldap_connect($host, $port);
$result = ldap_set_option($link, LDAP_OPT_X_TLS_CRLCHECK, 9001);
var_dump($result);
?>
===DONE===
--EXPECT--
bool(false)
===DONE===
