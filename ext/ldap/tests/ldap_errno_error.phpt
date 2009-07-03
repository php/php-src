--TEST--
ldap_errno() - ldap_errno() operation that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
// Too few parameters
var_dump(ldap_errno());

// Too many parameters
var_dump(ldap_errno(null, null));
?>
===DONE===
--EXPECTF--
Warning: Wrong parameter count for ldap_errno() in %s on line %d
NULL

Warning: Wrong parameter count for ldap_errno() in %s on line %d
NULL
===DONE===
