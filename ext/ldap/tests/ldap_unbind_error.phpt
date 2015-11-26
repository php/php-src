--TEST--
ldap_unbind() - ldap_unbind() operations that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifbindfailure.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);

// Too few parameters
var_dump(ldap_unbind());

// Too many parameters
var_dump(ldap_unbind($link, "Additional data"));

// Bad parameter
var_dump(ldap_unbind("string"));

// unbind twice
var_dump(ldap_unbind($link));
var_dump(ldap_unbind($link));
?>
===DONE===
--EXPECTF--
Warning: ldap_unbind() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)

Warning: ldap_unbind() expects exactly 1 parameter, 2 given in %s on line %d
bool(false)

Warning: ldap_unbind() expects parameter 1 to be resource, %unicode_string_optional% given in %s on line %d
bool(false)
bool(true)

Warning: ldap_unbind(): supplied resource is not a valid ldap link resource in %s on line %d
bool(false)
===DONE===
