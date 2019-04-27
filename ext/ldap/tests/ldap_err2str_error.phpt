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

var_dump(ldap_err2str("weird"));
?>
===DONE===
--EXPECTF--
Warning: ldap_err2str() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: ldap_err2str() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Warning: ldap_err2str() expects parameter 1 to be int, string given in %s on line %d
NULL
===DONE===
