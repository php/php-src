--TEST--
ldap_bind() - Binding that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--EXTENSIONS--
ldap
--SKIPIF--
<?php require_once('skipifbindfailure.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect($uri);
ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);

// Invalid password
var_dump(ldap_bind($link, $user, "ThisIsNotCorrect$passwd"));

// Invalid DN syntax
var_dump(ldap_bind($link, "unexistingProperty=weirdValue,$user", $passwd));
?>
--EXPECTF--
Warning: ldap_bind(): Unable to bind to server: Invalid credentials in %s on line %d
bool(false)

Warning: ldap_bind(): Unable to bind to server: Invalid DN syntax in %s on line %d
bool(false)
