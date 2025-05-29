--TEST--
ldap_start_tls() - Basic ldap_start_tls test
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--EXTENSIONS--
ldap
--ENV--
LDAPNOINIT=1
--SKIPIF--
<?php require_once __DIR__ .'/skipifbindfailure.inc'; ?>
--FILE--
<?php
require_once "connect.inc";

// CI uses self signed certificate

// No cert option - fails
$link = ldap_connect($uri);
ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);
var_dump(@ldap_start_tls($link));

// No cert check - passes
$link = ldap_connect($uri);
ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);
ldap_set_option($link, LDAP_OPT_X_TLS_REQUIRE_CERT, LDAP_OPT_X_TLS_NEVER);
var_dump(@ldap_start_tls($link));

// With cert check - fails
$link = ldap_connect($uri);
ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);
ldap_set_option($link, LDAP_OPT_X_TLS_REQUIRE_CERT, LDAP_OPT_X_TLS_DEMAND);
var_dump(@ldap_start_tls($link));
?>
--EXPECT--
bool(false)
bool(true)
bool(false)
