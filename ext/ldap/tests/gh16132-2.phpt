--TEST--
Bug GH-16132: Attempting to free pointer not allocated by ZMM
--EXTENSIONS--
ldap
--FILE--
<?php

/* ldap_add(_ext)(), ldap_mod_replace(_ext)(), ldap_mod_add(_ext)(), and ldap_mod_del(_ext)() share an underlying C function */
/* We are assuming 3333 is not connectable */
$ldap = ldap_connect('ldap://127.0.0.1:3333');
$valid_dn = "cn=userA,something";

$dict_key_multi_value_not_list_of_strings2 = [
    'attribute1' => 'value',
    'attribute2' => [
        'value1',
        new stdClass(),
    ],
];
try {
    var_dump(ldap_add($ldap, $valid_dn, $dict_key_multi_value_not_list_of_strings2));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Error: Object of class stdClass could not be converted to string
