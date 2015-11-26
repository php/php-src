--TEST--
ldap_start_tls() - Binding that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifbindfailure.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect($host, $port);
ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);

// Invalid parameter count
var_dump(ldap_start_tls());
var_dump(ldap_start_tls($link, $link));
?>
===DONE===
--EXPECTF--
Warning: ldap_start_tls() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: ldap_start_tls() expects exactly 1 parameter, 2 given in %s on line %d
NULL
===DONE===
