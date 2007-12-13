--TEST--
ldap_dn2ufn() test
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

/* Convert valid DN */
var_dump(ldap_dn2ufn("cn=bob,dc=example,dc=com"));

/* Convert valid DN */
var_dump(ldap_dn2ufn("cn=bob,ou=users,dc=example,dc=com"));

/* Convert DN with < > characters */
var_dump(ldap_dn2ufn("cn=<bob>,dc=example,dc=com"));

/* Too many parameters */
ldap_dn2ufn("cn=bob,dc=example,dc=com", 1);

/* Bad DN value */
var_dump(ldap_dn2ufn("bob,dc=example,dc=com"));

echo "Done\n";

?>
--EXPECTF--
string(16) "bob, example.com"
string(23) "bob, users, example.com"
bool(false)

Warning: Wrong parameter count for ldap_dn2ufn() in %s on line %d
bool(false)
Done
