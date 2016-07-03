--TEST--
ldap_escape() test ignore
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$subject = 'foo=bar(baz)*';
$ignore = 'ao';

var_dump(ldap_escape($subject, $ignore));

?>
--EXPECT--
string(31) "\66oo\3d\62a\72\28\62a\7a\29\2a"