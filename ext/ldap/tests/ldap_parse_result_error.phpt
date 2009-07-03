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
Warning: Wrong parameter count for ldap_parse_result() in %s on line %d
NULL
===DONE===
