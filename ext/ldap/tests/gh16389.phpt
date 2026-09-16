--TEST--
GH-16389 (Assertion failure in ext/ldap/ldap.c:2718)
--EXTENSIONS--
ldap
--FILE--
<?php
$attrib = "attrib";
$modtype = LDAP_MODIFY_BATCH_ADD;
$values = ["value1"];
$ldap = ldap_connect('ldap://127.0.0.1:3333');
$modification_attrib_reference_string = [
    [
        "attrib"  => &$attrib,
        "modtype" => &$modtype,
        "values"  => &$values,
    ],
];
var_dump(ldap_modify_batch($ldap, "", $modification_attrib_reference_string));
?>
--EXPECTF--
Warning: ldap_modify_batch(): Batch Modify: Can't contact LDAP server in %s on line %d
bool(false)
