--TEST--
ldap_err2str() - Basic error number to string conversion
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
var_dump(ldap_err2str(2));
?>
--EXPECT--
string(14) "Protocol error"
