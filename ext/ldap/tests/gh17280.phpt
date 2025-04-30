--TEST--
GH-17280 (ldap_search() fails when $attributes array has holes)
--EXTENSIONS--
ldap
--FILE--
<?php

/* We are assuming 3333 is not connectable */
$ldap = ldap_connect('ldap://127.0.0.1:3333');

// Creating an array with a hole in it
$attr = array_unique(['cn', 'uid', 'uid', 'mail']);
var_dump(ldap_search($ldap, 'ou=people,dc=example,dc=com', 'uid=admin', $attr));

?>
--EXPECTF--
Warning: ldap_search(): Search: Can't contact LDAP server in %s on line %d
bool(false)
