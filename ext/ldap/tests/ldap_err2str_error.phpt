--TEST--
ldap_err2str() - Incorrect usage of number to string conversion
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
// Too few args
var_dump(ldap_err2str());

// Too many args
var_dump(ldap_err2str(1, "Additional data"));
?>
===DONE===
--EXPECTF--
Warning: Wrong parameter count for ldap_err2str() in %s on line %d
NULL

Warning: Wrong parameter count for ldap_err2str() in %s on line %d
NULL
===DONE===
