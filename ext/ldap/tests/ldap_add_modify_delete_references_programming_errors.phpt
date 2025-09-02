--TEST--
Programming errors (Value/Type errors) for ldap_add(_ext)(), ldap_mod_replace(_ext)(), ldap_mod_add(_ext)(), and ldap_mod_del(_ext)() with references
--EXTENSIONS--
ldap
--FILE--
<?php

/* ldap_add(_ext)(), ldap_mod_replace(_ext)(), ldap_mod_add(_ext)(), and ldap_mod_del(_ext)() share an underlying C function */
/* We are assuming 3333 is not connectable */
$ldap = ldap_connect('ldap://127.0.0.1:3333');
$valid_dn = "cn=userA,something";

/* Taken from the existing ldap_add() PHP documentation */
$valid_entries = [
    'attribute1' => 'value',
    'attribute2' => [
        'value1',
        'value2',
    ],
];

$obj = new stdClass();
$dict_key_value_not_string = [
    'attribute1' => &$obj,
    'attribute2' => [
        'value1',
        'value2',
    ],
];
try {
    var_dump(ldap_add($ldap, $valid_dn, $dict_key_value_not_string));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$empty_list = [];
$dict_key_multi_value_empty_list = [
    'attribute1' => 'value',
    'attribute2' => &$empty_list,
];
try {
    var_dump(ldap_add($ldap, $valid_dn, $dict_key_multi_value_empty_list));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$empty_list = [];
$dict_key_multi_value_not_list_of_strings = [
    'attribute1' => 'value',
    'attribute2' => [
        'value1',
        &$empty_list,
    ],
];
try {
    var_dump(ldap_add($ldap, $valid_dn, $dict_key_multi_value_not_list_of_strings));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$obj = new stdClass();
$dict_key_multi_value_not_list_of_strings2 = [
    'attribute1' => 'value',
    'attribute2' => [
        'value1',
        &$obj,
    ],
];
try {
    var_dump(ldap_add($ldap, $valid_dn, $dict_key_multi_value_not_list_of_strings2));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
/* We don't check that values have nul bytes as the length of the string is passed to LDAP */

?>
--EXPECT--
Error: Object of class stdClass could not be converted to string
ValueError: ldap_add(): Argument #3 ($entry) attribute "attribute2" must be a non-empty list of attribute values
TypeError: LDAP value must be of type string|int|bool, array given
TypeError: LDAP value must be of type string|int|bool, stdClass given
