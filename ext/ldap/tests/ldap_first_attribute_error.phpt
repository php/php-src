--TEST--
ldap_first_attribute() - Testing ldap_first_attribute() that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect($host, $port);
var_dump(ldap_first_attribute($link));
var_dump(ldap_first_attribute($link, $link));
?>
===DONE===
--EXPECTF--
Warning: ldap_first_attribute() expects %s 2 parameters, 1 given in %s on line %d
NULL

Warning: ldap_first_attribute(): supplied resource is not a valid ldap result entry resource in %s on line %d
bool(false)
===DONE===
