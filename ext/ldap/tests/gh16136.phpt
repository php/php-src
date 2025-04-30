--TEST--
Bug GH-16136: Memory leak in php_ldap_do_modify() when entry is not a proper dictionary
--EXTENSIONS--
ldap
--FILE--
<?php

/* ldap_add(_ext)(), ldap_mod_replace(_ext)(), ldap_mod_add(_ext)(), and ldap_mod_del(_ext)() share an underlying C function */
/* We are assuming 3333 is not connectable */
$ldap = ldap_connect('ldap://127.0.0.1:3333');
$valid_dn = "cn=userA,something";

$not_dict_of_attributes = [
    'attribute1' => 'value',
    'not_key_entry',
    'attribute3' => [
        'value1',
        'value2',
    ],
];
try {
    var_dump(ldap_add($ldap, $valid_dn, $not_dict_of_attributes));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
Warning: ldap_add(): Unknown attribute in the data in %s on line %d
bool(false)
