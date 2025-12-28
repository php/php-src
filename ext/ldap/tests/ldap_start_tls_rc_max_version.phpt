--TEST--
ldap_start_tls() - Basic ldap_start_tls test
--EXTENSIONS--
ldap
--ENV--
LDAPCONF={PWD}/ldap_start_tls_rc_max_version.conf
--SKIPIF--
<?php
// Skip in CI for now as adding olcTLSProtocolMin does not seem to work (needs investigation)
if (getenv('CI')) {
    die("Skip in CI for now");
}
$require_vendor = [
    "name" => "OpenLDAP",
    "min_version" => 20600,
];
require_once __DIR__ .'/skipifbindfailure.inc';
?>
--FILE--
<?php
require_once "connect.inc";

// CI uses self signed certificate

// No cert option - fails
$link = ldap_connect($uri);
ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);
var_dump(@ldap_start_tls($link));

// No cert check - should pass but due to ldaps check, it fails as well
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
bool(false)
bool(false)
