--TEST--
ldap_get_option() - ldap_get_option() operation that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect($host, $port);
$option = null;

// Too few parameters
var_dump(ldap_get_option());
var_dump(ldap_get_option($link));
var_dump(ldap_get_option($link, LDAP_OPT_PROTOCOL_VERSION));

// Too many parameters
var_dump(
	ldap_get_option($link, LDAP_OPT_PROTOCOL_VERSION, $option, "Additional data"),
	$option
);
?>
===DONE===
--EXPECTF--
Warning: Wrong parameter count for ldap_get_option() in %s on line %d
NULL

Warning: Wrong parameter count for ldap_get_option() in %s on line %d
NULL

Warning: Wrong parameter count for ldap_get_option() in %s on line %d
NULL

Warning: Wrong parameter count for ldap_get_option() in %s on line %d
NULL
NULL
===DONE===
