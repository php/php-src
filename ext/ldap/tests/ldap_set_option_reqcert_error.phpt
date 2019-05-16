--TEST--
ldap_option_reqcert_basic() - Error test for TLS require cert ldap option
--CREDITS--
Edwin Hoksberg <edwin@edwinhoksberg.nl>
--SKIPIF--
<?php require_once __DIR__ .'/skipif.inc'; ?>
--FILE--
<?php
require "connect.inc";
$link = ldap_connect($host, $port);
$result = ldap_set_option($link, LDAP_OPT_X_TLS_REQUIRE_CERT, 9001);
var_dump($result);
?>
===DONE===
--EXPECT--
bool(false)
===DONE===
