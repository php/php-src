--TEST--
ldap_escape() test filter and DN
--EXTENSIONS--
ldap
--FILE--
<?php

$subject = 'foo=bar(baz)*';

var_dump(ldap_escape($subject, '', LDAP_ESCAPE_DN | LDAP_ESCAPE_FILTER));

?>
--EXPECT--
string(21) "foo\3dbar\28baz\29\2a"
