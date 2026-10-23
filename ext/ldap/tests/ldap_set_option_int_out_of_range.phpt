--TEST--
ldap_set_option() with an int option value out of the range of a C int
--EXTENSIONS--
ldap
--SKIPIF--
<?php
if (PHP_INT_SIZE <= 4) die('skip requires a 64-bit zend_long');
?>
--FILE--
<?php
foreach ([PHP_INT_MAX, PHP_INT_MIN] as $value) {
    try {
        var_dump(ldap_set_option(null, LDAP_OPT_SIZELIMIT, $value));
    } catch (ValueError $exception) {
        echo $exception->getMessage(), "\n";
    }
}
?>
--EXPECT--
ldap_set_option(): Argument #3 ($value) must be between -2147483648 and 2147483647
ldap_set_option(): Argument #3 ($value) must be between -2147483648 and 2147483647
