--TEST--
ldap_escape() test DN
--EXTENSIONS--
ldap
--FILE--
<?php

$subject = 'foo=bar(baz)*';

var_dump(ldap_escape($subject, '', LDAP_ESCAPE_DN));

?>
--EXPECT--
string(15) "foo\3dbar(baz)*"
