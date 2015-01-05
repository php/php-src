--TEST--
ldap_connect() - Connection errors
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--INI--
ldap.max_links=1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require "connect.inc";

// too many arguments
var_dump(ldap_connect(null, null, null));
var_dump(ldap_connect("ldap://$host:$port/dc=my-domain,dc=com"));

$links = array();
$links[0] = ldap_connect($host, $port);
$links[1] = ldap_connect($host, $port);
?>
===DONE===
--EXPECTF--
Warning: ldap_connect() expects at most 2 parameters, 3 given in %s on line %d
bool(false)

Warning: ldap_connect(): Could not create session handle: %s in %s on line %d
bool(false)

Warning: ldap_connect(): Too many open links (1) in %s on line %d
===DONE===
