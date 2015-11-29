--TEST--
ldap_sort() - Testing ldap_sort() that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect($host, $port);
var_dump(ldap_sort($link));
var_dump(ldap_sort($link, $link));
var_dump(ldap_sort($link, $link, $link, $link));
var_dump(ldap_sort($link, $link, $link));
var_dump(ldap_sort($link, $link, "sn"));
?>
===DONE===
--EXPECTF--
Deprecated: Function ldap_sort() is deprecated in %s.php on line %d

Warning: ldap_sort() expects exactly 3 parameters, 1 given in %s on line %d
bool(false)

Deprecated: Function ldap_sort() is deprecated in %s.php on line %d

Warning: ldap_sort() expects exactly 3 parameters, 2 given in %s on line %d
bool(false)

Deprecated: Function ldap_sort() is deprecated in %s.php on line %d

Warning: ldap_sort() expects exactly 3 parameters, 4 given in %s on line %d
bool(false)

Deprecated: Function ldap_sort() is deprecated in %s.php on line %d

Warning: ldap_sort() expects parameter 3 to be %binary_string_optional%, resource given in %s on line %d
bool(false)

Deprecated: Function ldap_sort() is deprecated in %s.php on line %d

Warning: ldap_sort(): Supplied resource is not a valid ldap result resource in %s on line %d
bool(false)
===DONE===
