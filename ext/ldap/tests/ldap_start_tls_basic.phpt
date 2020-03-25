--TEST--
ldap_start_tls() - Basic ldap_start_tls test
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once __DIR__ .'/skipif.inc'; ?>
<?php require_once __DIR__ .'/skipifbindfailure.inc'; ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect($host, $port);
ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);
var_dump(ldap_start_tls($link));
?>
--EXPECT--
bool(true)
