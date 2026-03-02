--TEST--
ldap_start_tls() - Basic ldap_start_tls test with TLS_CACERTFILE
--EXTENSIONS--
ldap
--SKIPIF--
<?php
require_once __DIR__ .'/skipifbindfailure.inc'; 
if (!ldap_get_option(NULL, LDAP_OPT_X_TLS_CACERTFILE, $val)) die('skip missing TLS_CACERTFILE');
?>
--FILE--
<?php
require_once "connect.inc";

// CI uses self signed certificate

// No cert option
$link = ldap_connect($uri);
ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);
var_dump(@ldap_start_tls($link));

// No cert check
$link = ldap_connect($uri);
ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);
ldap_set_option($link, LDAP_OPT_X_TLS_REQUIRE_CERT, LDAP_OPT_X_TLS_NEVER);
var_dump(@ldap_start_tls($link));

// With cert check
$link = ldap_connect($uri);
ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);
ldap_set_option($link, LDAP_OPT_X_TLS_REQUIRE_CERT, LDAP_OPT_X_TLS_DEMAND);
var_dump(@ldap_start_tls($link));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
