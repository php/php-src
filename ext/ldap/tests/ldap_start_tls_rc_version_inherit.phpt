--TEST--
ldap_start_tls() - TLS protocol version options from LDAPCONF are inherited
--EXTENSIONS--
ldap
--ENV--
LDAPCONF={PWD}/ldap_start_tls_rc_version_inherit.conf
--SKIPIF--
<?php
if (!defined('LDAP_OPT_X_TLS_PROTOCOL_MAX')) {
    die("skip Requires LDAP_OPT_X_TLS_PROTOCOL_MAX");
}
?>
--FILE--
<?php
require_once "connect.inc";

function dump_versions($link) {
    ldap_get_option($link, LDAP_OPT_X_TLS_PROTOCOL_MIN, $min);
    ldap_get_option($link, LDAP_OPT_X_TLS_PROTOCOL_MAX, $max);
    var_dump($min === LDAP_OPT_X_TLS_PROTOCOL_TLS1_0, $max);
}

// The result of ldap_start_tls() depends on the server and TLS backend so only options are checked

// Not inherited before TLS context is created
$link = ldap_connect($uri);
ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);
dump_versions($link);

// No TLS option set on the link
@ldap_start_tls($link);
dump_versions($link);

// Unrelated TLS option set on the link
$link = ldap_connect($uri);
ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);
ldap_set_option($link, LDAP_OPT_X_TLS_REQUIRE_CERT, LDAP_OPT_X_TLS_NEVER);
@ldap_start_tls($link);
dump_versions($link);

// Version set on the link is not overwritten
$link = ldap_connect($uri);
ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);
ldap_set_option($link, LDAP_OPT_X_TLS_PROTOCOL_MAX, LDAP_OPT_X_TLS_PROTOCOL_TLS1_2);
@ldap_start_tls($link);
dump_versions($link);

var_dump(LDAP_OPT_X_TLS_PROTOCOL_TLS1_1, LDAP_OPT_X_TLS_PROTOCOL_TLS1_2);
?>
--EXPECT--
bool(false)
int(0)
bool(true)
int(770)
bool(true)
int(770)
bool(true)
int(771)
int(770)
int(771)
