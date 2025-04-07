--TEST--
ldap_modify_batch() - modification array contains references
--EXTENSIONS--
ldap
--FILE--
<?php

/* We are assuming 3333 is not connectable */
$ldap = ldap_connect('ldap://127.0.0.1:3333');
$valid_dn = "cn=userA,something";

$empty_list = [];
$modification_reference = [
    &$empty_list,
];
try {
    var_dump(ldap_modify_batch($ldap, $valid_dn, $modification_reference));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$attrib = "attrib\0with\0nul\0byte";
$modification_attrib_reference_string = [
    [
        "attrib"  => &$attrib,
        "modtype" => LDAP_MODIFY_BATCH_ADD,
        "values"  => ["value1"],
    ],
];
try {
    var_dump(ldap_modify_batch($ldap, $valid_dn, $modification_attrib_reference_string));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$modtype = -10;
$modification_modtype_reference_int = [
    [
        "attrib"  => "attrib1",
        "modtype" => &$modtype,
        "values"  => ["value1"],
    ],
];
try {
    var_dump(ldap_modify_batch($ldap, $valid_dn, $modification_modtype_reference_int));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}


$values = [];
$modification_values_reference_array = [
    [
        "attrib"  => "attrib1",
        "modtype" => LDAP_MODIFY_BATCH_ADD,
        "values"  => &$values,
    ],
];
try {
    var_dump(ldap_modify_batch($ldap, $valid_dn, $modification_values_reference_array));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: ldap_modify_batch(): Argument #3 ($modifications_info) a modification entry must only contain the keys "attrib", "modtype", and "values"
ValueError: ldap_modify_batch(): Argument #3 ($modifications_info) the value for option "attrib" must not contain null bytes
ValueError: ldap_modify_batch(): Argument #3 ($modifications_info) the value for option "modtype" must be LDAP_MODIFY_BATCH_ADD, LDAP_MODIFY_BATCH_REMOVE, LDAP_MODIFY_BATCH_REPLACE, or LDAP_MODIFY_BATCH_REMOVE_ALL
ValueError: ldap_modify_batch(): Argument #3 ($modifications_info) the value for option "values" must not be empty
