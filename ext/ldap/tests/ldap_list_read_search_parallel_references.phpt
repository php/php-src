--TEST--
Programming errors (Value/Type errors) for parallel usage of ldap_list(), ldap_read(), and ldap_search() with references
--EXTENSIONS--
ldap
--FILE--
<?php

/* ldap_list(), ldap_read(), and ldap_search() share an underlying C function */
/* We are assuming 3333 is not connectable */
$ldap = ldap_connect('ldap://127.0.0.1:3333');
$valid_dn = "cn=userA,something";
$valid_filter = "";

$ldaps = [&$ldap, $ldap];

$str = "string\0with_nul_byte";

$list_with_ref_nul_byte = [
    &$str,
    "string2",
];

try {
    var_dump(ldap_list($ldaps, $list_with_ref_nul_byte, $valid_filter));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(ldap_list($ldaps, $valid_dn, $list_with_ref_nul_byte));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: ldap_list(): Argument #2 ($base) must not contain null bytes
ValueError: ldap_list(): Argument #3 ($filter) must not contain null bytes
