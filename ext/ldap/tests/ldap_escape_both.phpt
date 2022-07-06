--TEST--
ldap_escape() test filter and DN
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$subject = 'foo=bar(baz)*';

var_dump(ldap_escape($subject, null, LDAP_ESCAPE_DN | LDAP_ESCAPE_FILTER));

?>
--EXPECT--
string(21) "foo\3dbar\28baz\29\2a"
