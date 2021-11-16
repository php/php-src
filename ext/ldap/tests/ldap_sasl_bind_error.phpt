--TEST--
ldap_sasl_bind() - Binding that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--EXTENSIONS--
ldap
--SKIPIF--
<?php require_once('skipifbindfailure.inc'); ?>
<?php if (!function_exists("ldap_sasl_bind")) die("skip LDAP extension not compiled with SASL support"); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
insert_dummy_data($link, $base);
ldap_unbind($link);

$link = ldap_connect($host, $port);
ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);

// Invalid DN
var_dump(ldap_sasl_bind($link, "Invalid DN", $sasl_passwd, 'DIGEST-MD5', 'realm', $sasl_user));

// Invalid user
var_dump(ldap_sasl_bind($link, null, "ThisIsNotCorrect$sasl_passwd", 'DIGEST-MD5', "realm", "invalid$sasl_user"));

// Invalid password
var_dump(ldap_sasl_bind($link, null, "ThisIsNotCorrect$sasl_passwd", 'DIGEST-MD5', "realm", $sasl_user));

var_dump(ldap_sasl_bind($link, null, $sasl_passwd, 'DIGEST-MD5', "realm", "Manager", "test"));

// Invalid DN syntax
var_dump(ldap_sasl_bind($link, "unexistingProperty=weirdValue,$user", $sasl_passwd));
?>
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
remove_dummy_data($link, $base);
?>
--EXPECTF--
Warning: ldap_sasl_bind(): Unable to bind to server: Invalid DN syntax in %s on line %d
bool(false)

Warning: ldap_sasl_bind(): Unable to bind to server: Invalid credentials in %s on line %d
bool(false)

Warning: ldap_sasl_bind(): Unable to bind to server: Invalid credentials in %s on line %d
bool(false)

Warning: ldap_sasl_bind(): Unable to bind to server: Insufficient access in %s on line %d
bool(false)

Warning: ldap_sasl_bind(): Unable to bind to server: Invalid DN syntax in %s on line %d
bool(false)
