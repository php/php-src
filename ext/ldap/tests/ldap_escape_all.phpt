--TEST--
ldap_escape() test all
--EXTENSIONS--
ldap
--FILE--
<?php

$subject = 'foo=bar(baz)*';

var_dump(ldap_escape($subject));

?>
--EXPECT--
string(39) "\66\6f\6f\3d\62\61\72\28\62\61\7a\29\2a"
