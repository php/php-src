--TEST--
ldap_read() - operation that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
include "connect.inc";

$link = ldap_connect($host, $port);

// Too few parameters
var_dump(ldap_read());
var_dump(ldap_read($link));
var_dump(ldap_read($link, $link));

// Too many parameters
var_dump(ldap_read($link, "$base", "(objectClass=*)", array(), 0, 0, 0, 0 , "Additional data"));
?>
===DONE===
--EXPECTF--
Warning: ldap_read() expects at least 3 parameters, 0 given in %s on line %d
NULL

Warning: ldap_read() expects at least 3 parameters, 1 given in %s on line %d
NULL

Warning: ldap_read() expects at least 3 parameters, 2 given in %s on line %d
NULL

Warning: ldap_read() expects at most 8 parameters, 9 given in %s on line %d
NULL
===DONE===
