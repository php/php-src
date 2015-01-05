--TEST--
ldap_parse_result() - Testing ldap_parse_result() that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require "connect.inc";
var_dump(ldap_parse_result(null, null));
?>
===DONE===
--EXPECTF--
Warning: ldap_parse_result() expects at least 3 parameters, 2 given in %s on line %d
NULL
===DONE===
