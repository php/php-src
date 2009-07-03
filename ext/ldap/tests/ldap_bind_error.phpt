--TEST--
ldap_bind() - Binding that should fail
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
var_dump(ldap_bind($link, $user, $passwd, null));

// Invalid password
var_dump(ldap_bind($link, $user, "ThisIsNotCorrect$passwd"));

// Invalid DN syntax
var_dump(ldap_bind($link, "unexistingProperty=weirdValue,$user", $passwd));
?>
===DONE===
--EXPECTF--
Warning: ldap_bind() expects at most 3 parameters, 4 given in %s on line %d
bool(false)

Warning: ldap_bind(): Unable to bind to server: Invalid credentials in %s on line %d
bool(false)

Warning: ldap_bind(): Unable to bind to server: Invalid DN syntax in %s on line %d
bool(false)
===DONE===
