--TEST--
ldap_dn2ufn() test
--EXTENSIONS--
ldap
--FILE--
<?php

/* Convert valid DN */
var_dump(ldap_dn2ufn("cn=bob,dc=example,dc=com"));

/* Convert valid DN */
var_dump(ldap_dn2ufn("cn=bob,ou=users,dc=example,dc=com"));

/* Convert DN with < > characters */
var_dump(ldap_dn2ufn("cn=<bob>,dc=example,dc=com"));

/* Bad DN value */
var_dump(ldap_dn2ufn("bob,dc=example,dc=com"));

echo "Done\n";

?>
--EXPECT--
string(16) "bob, example.com"
string(23) "bob, users, example.com"
bool(false)
bool(false)
Done
