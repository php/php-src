--TEST--
ldap_connect() - Variation
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require "connect.inc";

// no hostname, no port
$link = ldap_connect();
var_dump($link);

// no port
$link = ldap_connect($host);
var_dump($link);

// URI
$link = ldap_connect("ldap://$host:$port");
var_dump($link);

// URI no port
$link = ldap_connect("ldap://$host");
var_dump($link);

// bad hostname (connect should work, not bind)
$link = ldap_connect("nonexistent" . $host);
var_dump($link);

// Legacy host:port syntax
$link = ldap_connect("$host:$port");
var_dump($link);
?>
===DONE===
--EXPECTF--
resource(%d) of type (ldap link)
resource(%d) of type (ldap link)
resource(%d) of type (ldap link)
resource(%d) of type (ldap link)
resource(%d) of type (ldap link)
resource(%d) of type (ldap link)
===DONE===
