--TEST--
ldap_connect() - Basic ldaps test
--EXTENSIONS--
ldap
--ENV--
LDAPNOINIT=1
--SKIPIF--
<?php require_once __DIR__ .'/skipifbindfailure.inc'; ?>
--FILE--
<?php
require_once "connect.inc";

$uri = "ldaps://$host:636";

// CI uses self signed certificate

// No cert option - fails
$link = ldap_connect($uri);
ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);
var_dump(@ldap_bind($link, $user, $passwd));
ldap_unbind($link);

// No cert check - passes
ldap_set_option(null, LDAP_OPT_X_TLS_REQUIRE_CERT, LDAP_OPT_X_TLS_ALLOW);
$link = ldap_connect($uri);
ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);
var_dump(@ldap_bind($link, $user, $passwd));
ldap_unbind($link);

// No change to TLS options
$link = ldap_connect($uri);
ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);
var_dump(@ldap_bind($link, $user, $passwd));
ldap_unbind($link);

// With cert check - fails
ldap_set_option(null, LDAP_OPT_X_TLS_REQUIRE_CERT, LDAP_OPT_X_TLS_DEMAND);
$link = ldap_connect($uri);
ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);
var_dump(@ldap_bind($link, $user, $passwd));
ldap_unbind($link);

// No change to TLS options
$link = ldap_connect($uri);
ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);
var_dump(@ldap_bind($link, $user, $passwd));
ldap_unbind($link);

?>
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
