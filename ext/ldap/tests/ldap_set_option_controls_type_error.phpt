--TEST--
ldap_set_option(): the type error for the controls options names the option
--EXTENSIONS--
ldap
--FILE--
<?php
$l = ldap_connect('ldap://127.0.0.1:1');
foreach ([LDAP_OPT_SERVER_CONTROLS, LDAP_OPT_CLIENT_CONTROLS] as $option) {
    try {
        ldap_set_option($l, $option, "x");
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
TypeError: ldap_set_option(): Argument #3 ($value) must be of type array for the LDAP_OPT_SERVER_CONTROLS option, string given
TypeError: ldap_set_option(): Argument #3 ($value) must be of type array for the LDAP_OPT_CLIENT_CONTROLS option, string given
