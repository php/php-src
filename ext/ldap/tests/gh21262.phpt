--TEST--
GH-21262 (ldap_modify() too strict controls argument validation)
--EXTENSIONS--
ldap
--FILE--
<?php
/* We are assuming 3333 is not connectable */
$ldap = ldap_connect('ldap://127.0.0.1:3333');
$valid_dn = "cn=userA,something";

$entry_with_empty_array = [
	'attribute1' => 'value',
	'attribute2' => [],
];

// ldap_add() should still reject empty arrays
try {
	ldap_add($ldap, $valid_dn, $entry_with_empty_array);
} catch (ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}

// ldap_mod_add() should still reject empty arrays
try {
	ldap_mod_add($ldap, $valid_dn, $entry_with_empty_array);
} catch (ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}

// ldap_modify() should accept empty arrays (delete attribute)
try {
	@ldap_modify($ldap, $valid_dn, $entry_with_empty_array);
	echo "ldap_modify: no ValueError thrown", PHP_EOL;
} catch (ValueError $e) {
	echo "ldap_modify: UNEXPECTED ValueError: ", $e->getMessage(), PHP_EOL;
}

// ldap_mod_del() should accept empty arrays (delete attribute)
try {
	@ldap_mod_del($ldap, $valid_dn, $entry_with_empty_array);
	echo "ldap_mod_del: no ValueError thrown", PHP_EOL;
} catch (ValueError $e) {
	echo "ldap_mod_del: UNEXPECTED ValueError: ", $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
ldap_add(): Argument #3 ($entry) attribute "attribute2" must be a non-empty list of attribute values
ldap_mod_add(): Argument #3 ($entry) attribute "attribute2" must be a non-empty list of attribute values
ldap_modify: no ValueError thrown
ldap_mod_del: no ValueError thrown
