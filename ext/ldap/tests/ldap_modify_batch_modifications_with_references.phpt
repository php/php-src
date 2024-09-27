--TEST--
ldap_modify_batch() - modification array contains references
--EXTENSIONS--
ldap
--FILE--
<?php

/* We are assuming 3333 is not connectable */
$ldap = ldap_connect('ldap://127.0.0.1:3333');
$valid_dn = "cn=userA,something";

$attrib = "attrib1";
$r =& $attrib;
$modification_attrib_reference_string = [
    [
        "attrib"  => $r,
        "modtype" => LDAP_MODIFY_BATCH_ADD,
        "values"  => ["value1"],
    ],
];
try {
    var_dump(ldap_modify_batch($ldap, $valid_dn, $modification_attrib_reference_string));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$modtype = LDAP_MODIFY_BATCH_ADD;
$r =& $modtype;
$modification_modtype_reference_int = [
    [
        "attrib"  => "attrib1",
        "modtype" => $r,
        "values"  => ["value1"],
    ],
];
try {
    var_dump(ldap_modify_batch($ldap, $valid_dn, $modification_modtype_reference_int));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}


$values = ["value1"];
$r =& $values;
$modification_values_reference_array = [
    [
        "attrib"  => "attrib1",
        "modtype" => LDAP_MODIFY_BATCH_ADD,
        "values"  => $r,
    ],
];
try {
    var_dump(ldap_modify_batch($ldap, $valid_dn, $modification_values_reference_array));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
Warning: ldap_modify_batch(): Batch Modify: Can't contact LDAP server in %s on line %d
bool(false)

Warning: ldap_modify_batch(): Batch Modify: Can't contact LDAP server in %s on line %d
bool(false)

Warning: ldap_modify_batch(): Batch Modify: Can't contact LDAP server in %s on line %d
bool(false)
