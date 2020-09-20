--TEST--
ldap_option_reqcert_basic() - Basic test to check if PHP can connect to a LDAP server with an invalid certificate with certificate checking disabled
--CREDITS--
Edwin Hoksberg <edwin@edwinhoksberg.nl>
--SKIPIF--
<?php
    require_once __DIR__ .'/skipif.inc';
    require_once __DIR__ .'/skipifbindfailure.inc';
?>
--FILE--
<?php
require "connect.inc";

ldap_set_option(null, LDAP_OPT_X_TLS_REQUIRE_CERT, LDAP_OPT_X_TLS_NEVER);

$link = ldap_connect($host, $port);
ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);

var_dump(@ldap_bind($link, $user, $passwd));
?>
--EXPECT--
bool(true)
