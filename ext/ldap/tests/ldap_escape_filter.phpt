--TEST--
ldap_escape() test filter
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$subject = 'foo=bar(baz)*';

var_dump(ldap_escape($subject, null, LDAP_ESCAPE_FILTER));

?>
--EXPECT--
string(19) "foo=bar\28baz\29\2a"
