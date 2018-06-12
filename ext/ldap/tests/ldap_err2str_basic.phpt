--TEST--
ldap_err2str() - Basic error number to string conversion
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php
var_dump(ldap_err2str(2));
?>
===DONE===
--EXPECT--
string(14) "Protocol error"
===DONE===
