--TEST--
ldap_list() - operation that should fail
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
var_dump(ldap_list());
var_dump(ldap_list($link));
var_dump(ldap_list($link, $link));

// Too many parameters
var_dump(ldap_list($link, "dc=my-domain,dc=com", "(objectClass=*)", array(), 0, 0, 0, 0 , "Additional data"));
?>
===DONE===
--EXPECTF--
Warning: Wrong parameter count for ldap_list() in %s on line %d
NULL

Warning: Wrong parameter count for ldap_list() in %s on line %d
NULL

Warning: Wrong parameter count for ldap_list() in %s on line %d
NULL

Warning: Wrong parameter count for ldap_list() in %s on line %d
NULL
===DONE===
