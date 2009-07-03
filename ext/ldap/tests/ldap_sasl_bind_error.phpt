--TEST--
ldap_sasl_bind() - Binding that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifbindfailure.inc'); ?>
<?php if (!function_exists("ldap_sasl_bind")) die("skip LDAP extension not compiled with SASL support"); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect($host, $port);
ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);

// Invalid parameter count
var_dump(ldap_sasl_bind());

// Invalid DN
var_dump(ldap_sasl_bind($link, "Invalid DN", $passwd, 'DIGEST-MD5', 'realm', $sasl_user));

// Invalid user
var_dump(ldap_sasl_bind($link, null, "ThisIsNotCorrect$passwd", 'DIGEST-MD5', "realm", "invalid$sasl_user"));

// Invalid password
var_dump(ldap_sasl_bind($link, null, "ThisIsNotCorrect$passwd", 'DIGEST-MD5', "realm", $sasl_user));

var_dump(ldap_sasl_bind($link, null, $passwd, 'DIGEST-MD5', "realm", "Manager", "test"));

// Invalid DN syntax
var_dump(ldap_sasl_bind($link, "unexistingProperty=weirdValue,$user", $passwd));
?>
===DONE===
--EXPECTF--
Warning: ldap_sasl_bind() expects at least 1 parameter, 0 given in %s on line %d
bool(false)

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
===DONE===
