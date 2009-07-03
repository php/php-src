--TEST--
ldap_error() - ldap_error() operation that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
// Too few parameters
var_dump(ldap_error());

// Too many parameters
var_dump(ldap_error(null, null));
?>
===DONE===
--EXPECTF--
Warning: Wrong parameter count for ldap_error() in %s on line %d
NULL

Warning: Wrong parameter count for ldap_error() in %s on line %d
NULL
===DONE===
