--TEST--
ldap_option_reqcert_basic() - Error test for TLS require cert ldap option
--CREDITS--
Edwin Hoksberg <edwin@edwinhoksberg.nl>
--EXTENSIONS--
ldap
--FILE--
<?php
require "connect.inc";
$link = ldap_connect($uri);
$result = ldap_set_option($link, LDAP_OPT_X_TLS_REQUIRE_CERT, 9001);
var_dump($result);
?>
--EXPECT--
bool(false)
