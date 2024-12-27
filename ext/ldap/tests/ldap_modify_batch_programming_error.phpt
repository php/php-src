--TEST--
ldap_modify_batch() - ValueErrors and TypeErrors
--EXTENSIONS--
ldap
--FILE--
<?php

/* We are assuming 3333 is not connectable */
$ldap = ldap_connect('ldap://127.0.0.1:3333');

$valid_dn = "cn=userA,something";

$dn = "dn_with\0nul_byte";
try {
    var_dump(ldap_modify_batch($ldap, $dn, []));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$empty_list = [];
try {
    var_dump(ldap_modify_batch($ldap, $valid_dn, $empty_list));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$not_list1 = [
    'entry1' => [],
    'entry2' => [],
];
try {
    var_dump(ldap_modify_batch($ldap, $valid_dn, $not_list1));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$not_list2 = [
    [
        "attrib"  => "attrib1",
        "modtype" => LDAP_MODIFY_BATCH_ADD,
        "values"  => ["value1"],
    ],
    2 => [],
];
try {
    var_dump(ldap_modify_batch($ldap, $valid_dn, $not_list2));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}


$not_list_of_arrays = [
    [
        "attrib"  => "attrib1",
        "modtype" => LDAP_MODIFY_BATCH_ADD,
        "values"  => ["value1"],
    ],
    'not an array',
];
try {
    var_dump(ldap_modify_batch($ldap, $valid_dn, $not_list_of_arrays));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$modifications_not_all_dicts = [
    [
        "attrib"  => "attrib1",
        "modtype" => LDAP_MODIFY_BATCH_ADD,
        "values"  => ["value1"],
    ],
    [
        "attrib"  => "attrib2",
        "modtype" => LDAP_MODIFY_BATCH_REMOVE_ALL,
        4         => ["value2"],
    ],
    [
        "attrib"  => "attrib3",
        "modtype" => LDAP_MODIFY_BATCH_ADD,
        "values"  => ["value3"],
    ],
];
try {
    var_dump(ldap_modify_batch($ldap, $valid_dn, $modifications_not_all_dicts));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$modification_has_invalid_key = [
    [
        "attrib"  => "attrib1",
        "modtype" => LDAP_MODIFY_BATCH_ADD,
        "values"  => ["value1"],
        "random"  => "what",
    ],
];
try {
    var_dump(ldap_modify_batch($ldap, $valid_dn, $modification_has_invalid_key));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$modification_attrib_not_string = [
    [
        "attrib"  => 25,
        "modtype" => LDAP_MODIFY_BATCH_ADD,
        "values"  => ["value1"],
    ],
];
try {
    var_dump(ldap_modify_batch($ldap, $valid_dn, $modification_attrib_not_string));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$modification_attrib_has_nul_byte = [
    [
        "attrib"  => "attrib\0with\0nul\0byte",
        "modtype" => LDAP_MODIFY_BATCH_ADD,
        "values"  => ["value1"],
    ],
];
try {
    var_dump(ldap_modify_batch($ldap, $valid_dn, $modification_attrib_has_nul_byte));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$modification_modtype_not_int = [
    [
        "attrib"  => "attrib1",
        "modtype" => new stdClass(),
        "values"  => ["value1"],
    ],
];
try {
    var_dump(ldap_modify_batch($ldap, $valid_dn, $modification_modtype_not_int));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$modification_modtype_invalid_value = [
    [
        "attrib"  => "attrib1",
        "modtype" => 42,
        "values"  => ["value1"],
    ],
];
try {
    var_dump(ldap_modify_batch($ldap, $valid_dn, $modification_modtype_invalid_value));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$modification_modtype_remove_all_with_values_key = [
    [
        "attrib"  => "attrib1",
        "modtype" => LDAP_MODIFY_BATCH_REMOVE_ALL,
        "values"  => ["value1"],
    ],
];
try {
    var_dump(ldap_modify_batch($ldap, $valid_dn, $modification_modtype_remove_all_with_values_key));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$modification_modtype_not_remove_all_without_values_key = [
    [
        "attrib"  => "attrib1",
        "modtype" => LDAP_MODIFY_BATCH_ADD,
    ],
];
try {
    var_dump(ldap_modify_batch($ldap, $valid_dn, $modification_modtype_not_remove_all_without_values_key));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$modification_values_invalid_value = [
    [
        "attrib"  => "attrib1",
        "modtype" => LDAP_MODIFY_BATCH_ADD,
        "values"  => "value1",
    ],
];
try {
    var_dump(ldap_modify_batch($ldap, $valid_dn, $modification_values_invalid_value));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$modification_values_empty_array = [
    [
        "attrib"  => "attrib1",
        "modtype" => LDAP_MODIFY_BATCH_ADD,
        "values"  => [],
    ],
];
try {
    var_dump(ldap_modify_batch($ldap, $valid_dn, $modification_values_empty_array));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$modification_values_not_list1 = [
    [
        "attrib"  => "attrib1",
        "modtype" => LDAP_MODIFY_BATCH_ADD,
        "values"  => $not_list1,
    ],
];
try {
    var_dump(ldap_modify_batch($ldap, $valid_dn, $modification_values_not_list1));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$modification_missing_attrib_key = [
    [
        "modtype" => LDAP_MODIFY_BATCH_ADD,
        "values"  => ["value1"],
    ],
];
try {
    var_dump(ldap_modify_batch($ldap, $valid_dn, $modification_missing_attrib_key));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$modification_missing_modtype_key = [
    [
        "attrib"  => "attrib1",
        "values"  => ["value1"],
    ],
];
try {
    var_dump(ldap_modify_batch($ldap, $valid_dn, $modification_missing_modtype_key));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: ldap_modify_batch(): Argument #2 ($dn) must not contain any null bytes
ValueError: ldap_modify_batch(): Argument #3 ($modifications_info) must not be empty
ValueError: ldap_modify_batch(): Argument #3 ($modifications_info) must be an array with numeric keys
ValueError: ldap_modify_batch(): Argument #3 ($modifications_info) a modification entry must only contain the keys "attrib", "modtype", and "values"
TypeError: ldap_modify_batch(): Argument #3 ($modifications_info) must only contain arrays
ValueError: ldap_modify_batch(): Argument #3 ($modifications_info) a modification entry must not contain the "values" option when option "modtype" is LDAP_MODIFY_BATCH_REMOVE_ALL
ValueError: ldap_modify_batch(): Argument #3 ($modifications_info) a modification entry must only contain the keys "attrib", "modtype", and "values"
TypeError: ldap_modify_batch(): Argument #3 ($modifications_info) the value for option "attrib" must be of type string, int given
ValueError: ldap_modify_batch(): Argument #3 ($modifications_info) the value for option "attrib" must not contain null bytes
TypeError: ldap_modify_batch(): Argument #3 ($modifications_info) the value for option "modtype" must be of type int, string given
ValueError: ldap_modify_batch(): Argument #3 ($modifications_info) the value for option "modtype" must be LDAP_MODIFY_BATCH_ADD, LDAP_MODIFY_BATCH_REMOVE, LDAP_MODIFY_BATCH_REPLACE, or LDAP_MODIFY_BATCH_REMOVE_ALL
ValueError: ldap_modify_batch(): Argument #3 ($modifications_info) a modification entry must not contain the "values" option when option "modtype" is LDAP_MODIFY_BATCH_REMOVE_ALL
ValueError: ldap_modify_batch(): Argument #3 ($modifications_info) a modification entry must contain the "values" option when the "modtype" option is not LDAP_MODIFY_BATCH_REMOVE_ALL
TypeError: ldap_modify_batch(): Argument #3 ($modifications_info) the value for option "values" must be of type array, string given
ValueError: ldap_modify_batch(): Argument #3 ($modifications_info) the value for option "values" must not be empty
ValueError: ldap_modify_batch(): Argument #3 ($modifications_info) the value for option "values" must be an array with numeric keys
ValueError: ldap_modify_batch(): Argument #3 ($modifications_info) a modification entry must contain the "attrib" option
ValueError: ldap_modify_batch(): Argument #3 ($modifications_info) a modification entry must contain the "modtype" option
