--TEST--
ldap_connect() - Basic connection
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect($host, $port);
var_dump($link);
?>
===DONE===
--EXPECTF--
resource(%d) of type (ldap link)
===DONE===
