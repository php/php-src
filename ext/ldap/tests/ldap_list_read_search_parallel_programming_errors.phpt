--TEST--
Programming errors (Value/Type errors) for parallel usage of ldap_list(), ldap_read(), and ldap_search()
--EXTENSIONS--
ldap
--FILE--
<?php

/* ldap_list(), ldap_read(), and ldap_search() share an underlying C function */
/* We are assuming 3333 is not connectable */
$ldap = ldap_connect('ldap://127.0.0.1:3333');
$valid_dn = "cn=userA,something";
$valid_filter = "";

$ldaps = [$ldap, $ldap];

$dn = "dn_with\0nul_byte";
try {
    var_dump(ldap_list($ldaps, $dn, $valid_filter));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$filter = "filter_with\0nul_byte";
try {
    var_dump(ldap_list($ldaps, $valid_dn, $filter));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$not_list = [
    "attrib1",
    "wat" => "attrib2",
];
try {
    var_dump(ldap_list($ldaps, $not_list, $valid_filter));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(ldap_list($ldaps, $valid_dn, $not_list));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$list_not_same_length = [
    "string1",
    "string2",
    "string3",
];
try {
    var_dump(ldap_list($ldaps, $list_not_same_length, $valid_filter));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(ldap_list($ldaps, $valid_dn, $list_not_same_length));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: ldap_list(): Argument #2 ($base) must not contain null bytes
ValueError: ldap_list(): Argument #3 ($filter) must not contain null bytes
ValueError: ldap_list(): Argument #2 ($base) must be a list
ValueError: ldap_list(): Argument #3 ($filter) must be a list
ValueError: ldap_list(): Argument #2 ($base) must be the same size as argument #1
ValueError: ldap_list(): Argument #3 ($filter) must be the same size as argument #1
