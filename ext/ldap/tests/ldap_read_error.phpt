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
var_dump(ldap_read($link, "dc=my-domain,dc=com", "(objectClass=*)", array(), 0, 0, 0, 0 , "Additional data"));
?>
===DONE===
--EXPECTF--
Warning: Wrong parameter count for ldap_read() in %s on line %d
NULL

Warning: Wrong parameter count for ldap_read() in %s on line %d
NULL

Warning: Wrong parameter count for ldap_read() in %s on line %d
NULL

Warning: Wrong parameter count for ldap_read() in %s on line %d
NULL
===DONE===
