--TEST--
Bug GH-16101: Segfault in ldap_list(), ldap_read(), and ldap_search() when LDAPs array is not a list
--EXTENSIONS--
ldap
--FILE--
<?php

/* We are assuming 3333 is not connectable */
$ldap = ldap_connect('ldap://127.0.0.1:3333');
$valid_dn = "cn=userA,something";
$valid_filter = "";

$ldaps_dict = [
    "hello"  => $ldap,
    "world"  => $ldap,
];
try {
    var_dump(ldap_list($ldaps_dict, $valid_dn, $valid_filter));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: ldap_list(): Argument #1 ($ldap) must be a list
