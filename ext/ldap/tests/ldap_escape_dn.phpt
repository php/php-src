--TEST--
ldap_escape() test DN
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$subject = 'foo=bar(baz)*';

var_dump(ldap_escape($subject, null, LDAP_ESCAPE_DN));

?>
--EXPECT--
string(15) "foo\3dbar(baz)*"