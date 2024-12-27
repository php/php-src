--TEST--
Programming errors (Value/Type errors) for ldap_list(), ldap_read(), and ldap_search()
--EXTENSIONS--
ldap
--FILE--
<?php

/* ldap_list(), ldap_read(), and ldap_search() share an underlying C function */
/* We are assuming 3333 is not connectable */
$ldap = ldap_connect('ldap://127.0.0.1:3333');
$valid_dn = "cn=userA,something";
$valid_filter = "";

try {
    var_dump(ldap_list(42, $valid_dn, $valid_filter));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump(ldap_list($ldap, [$valid_dn], $valid_filter));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump(ldap_list($ldap, $valid_dn, [$valid_filter]));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$not_list = [
    "attrib1",
    "wat" => "attrib2",
    "attrib3",
];
try {
    var_dump(ldap_list($ldap, $valid_dn, $valid_filter, $not_list));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$not_list_of_strings = [
    "attrib1",
    42,
    "attrib3",
];
try {
    var_dump(ldap_list($ldap, $valid_dn, $valid_filter, $not_list_of_strings));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$list_of_strings_with_null_byte = [
    "attrib1",
    "attrib_with\0nul_byte",
    "attrib3",
];
try {
    var_dump(ldap_list($ldap, $valid_dn, $valid_filter, $list_of_strings_with_null_byte));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$str = "attrib_with\0nul_byte";

$list_with_ref_nul_byte = [
    "attrib1",
    &$str,
    "attrib3",
];
try {
    var_dump(ldap_list($ldap, $valid_dn, $valid_filter, $list_with_ref_nul_byte));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: ldap_list(): Argument #1 ($ldap) must be of type LDAP\Connection|array, int given
TypeError: ldap_list(): Argument #2 ($base) must be of type string when argument #1 ($ldap) is an LDAP\Connection instance
TypeError: ldap_list(): Argument #3 ($filter) must be of type string when argument #1 ($ldap) is an LDAP\Connection instance
ValueError: ldap_list(): Argument #4 ($attributes) must be an array with numeric keys
TypeError: ldap_list(): Argument #4 ($attributes) must be a list of strings, int given
ValueError: ldap_list(): Argument #4 ($attributes) must not contain strings with any null bytes
ValueError: ldap_list(): Argument #4 ($attributes) must not contain strings with any null bytes
