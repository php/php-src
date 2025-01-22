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

$not_list_ldaps = [
    "string1",
    $ldap,
];
try {
    var_dump(ldap_list($not_list_ldaps, $valid_dn, $valid_filter));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

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

$list_not_all_strings = [
    42,
    "string2",
];
try {
    var_dump(ldap_list($ldaps, $list_not_all_strings, $valid_filter));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(ldap_list($ldaps, $valid_dn, $list_not_all_strings));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$list_strings_with_nul_bytes = [
    "string\0nul_byte",
    "string2",
];
try {
    var_dump(ldap_list($ldaps, $list_strings_with_nul_bytes, $valid_filter));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(ldap_list($ldaps, $valid_dn, $list_strings_with_nul_bytes));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: ldap_list(): Argument #1 ($ldap) must be a list of LDAP\Connection
ValueError: ldap_list(): Argument #2 ($base) must not contain null bytes
ValueError: ldap_list(): Argument #3 ($filter) must not contain null bytes
ValueError: ldap_list(): Argument #2 ($base) must be a list
ValueError: ldap_list(): Argument #3 ($filter) must be a list
ValueError: ldap_list(): Argument #2 ($base) must be the same size as argument #1
ValueError: ldap_list(): Argument #3 ($filter) must be the same size as argument #1
TypeError: ldap_list(): Argument #2 ($base) must be a list of strings, int given
TypeError: ldap_list(): Argument #3 ($filter) must be a list of strings, int given
ValueError: ldap_list(): Argument #2 ($base) must not contain null bytes
ValueError: ldap_list(): Argument #3 ($filter) must not contain null bytes