--TEST--
ldap_(g|s)et_option() with non existing option
--EXTENSIONS--
ldap
--FILE--
<?php

$ldap = ldap_connect('ldap://127.0.0.1:3333');
try {
    var_dump(ldap_set_option($ldap, 999999, "bogus"));
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
try {
    var_dump(ldap_get_option($ldap, 999999, $value));
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
ValueError: ldap_set_option(): Argument #2 ($option) must be a valid LDAP option
ValueError: ldap_get_option(): Argument #2 ($option) must be a valid LDAP option
